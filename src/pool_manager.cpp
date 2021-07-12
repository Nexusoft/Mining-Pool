#include "pool_manager.hpp"
#include "wallet_connection.hpp"
#include "miner_connection.hpp"
#include "config/config.hpp"

namespace nexuspool
{

Pool_manager::Pool_manager(std::shared_ptr<asio::io_context> io_context, config::Config& config, network::Socket_factory::Sptr socket_factory)
	: m_io_context{std::move(io_context) }
	, m_config{config}
	, m_timer_factory{std::make_shared<chrono::Timer_factory>(m_io_context)}
	, m_socket_factory{std::move(socket_factory)}
	, m_logger{ spdlog::get("logger") }
	, m_listen_socket{}
	, m_session_registry{m_config.get_session_expiry_time()}
	, m_current_height{0}
{
	m_session_registry_maintenance = m_timer_factory->create_timer();
}

void Pool_manager::start()
{
	network::Endpoint wallet_endpoint{ network::Transport_protocol::tcp, m_config.get_wallet_ip(), m_config.get_wallet_port() };
	network::Endpoint local_endpoint{ network::Transport_protocol::tcp, m_config.get_local_ip(), m_config.get_local_port() };
	auto local_socket = m_socket_factory->create_socket(local_endpoint);

	auto self = shared_from_this();
	// connect to wallet
	m_wallet_connection = std::make_shared<Wallet_connection>(m_io_context, self, m_config, m_timer_factory, std::move(local_socket));
	if (!m_wallet_connection->connect(wallet_endpoint))
	{
		m_logger->critical("Couldn't connect to wallet using ip {} and port {}", m_config.get_wallet_ip(), m_config.get_wallet_port());
		return;
	}

	// listen
	network::Endpoint local_listen_endpoint{ network::Transport_protocol::tcp, m_config.get_local_ip(), m_config.get_local_listen_port() };
	m_listen_socket = m_socket_factory->create_socket(local_listen_endpoint);

	
	// on listen/accept, save created connection to pool_conenctions and call the connection_handler of created pool connection object
	auto socket_handler = [self](network::Connection::Sptr&& connection)
	{
		auto const session_key = self->m_session_registry.create_session();
		auto miner_connection = std::make_shared<Miner_connection>(self->m_timer_factory, std::move(connection), self, session_key, self->m_session_registry);

		auto session = self->m_session_registry.get_session(session_key);
		session.update_connection(miner_connection);
		self->m_session_registry.update_session(session_key, session);

		return miner_connection->connection_handler();
	};

	m_listen_socket->listen(socket_handler);

	m_session_registry_maintenance->start(chrono::Seconds(m_config.get_session_expiry_time()), 
		session_registry_maintenance_handler(m_config.get_session_expiry_time()));
	
}

void Pool_manager::stop()
{
	m_session_registry_maintenance->cancel();
	m_session_registry.stop();	// clear sessions and deletes miner_connection objects
	m_listen_socket->stop_listen();
}

chrono::Timer::Handler Pool_manager::session_registry_maintenance_handler(std::uint16_t session_registry_maintenance_interval)
{
	return[this, session_registry_maintenance_interval](bool canceled)
	{
		if (canceled)	// don't do anything if the timer has been canceled
		{
			return;
		}

		m_session_registry.clear_unused_sessions();

		// restart timer
		m_session_registry_maintenance->start(chrono::Seconds(session_registry_maintenance_interval),
			session_registry_maintenance_handler(session_registry_maintenance_interval));
	};

}

}