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
	, m_session_registry{}
{
}

void Pool_manager::start()
{
	network::Endpoint wallet_endpoint{ network::Transport_protocol::tcp, m_config.get_wallet_ip(), m_config.get_wallet_port() };
	network::Endpoint local_endpoint{ network::Transport_protocol::tcp, m_config.get_local_ip(), m_config.get_local_port() };
	auto local_socket = m_socket_factory->create_socket(local_endpoint);

	// connect to wallet
	m_wallet_connection = std::make_shared<Wallet_connection>(m_io_context, m_config, m_timer_factory, std::move(local_socket));
	if (!m_wallet_connection->connect(wallet_endpoint))
	{
		m_logger->critical("Couldn't connect to wallet using ip {} and port {}", m_config.get_wallet_ip(), m_config.get_wallet_port());
		return;
	}

	// listen
	network::Endpoint local_listen_endpoint{ network::Transport_protocol::tcp, m_config.get_local_ip(), m_config.get_local_listen_port() };
	m_listen_socket = m_socket_factory->create_socket(local_listen_endpoint);

	
	// on listen/accept, save created connection to pool_conenctions and call the connection_handler of created pool connection object
	auto socket_handler = [this](network::Connection::Sptr&& connection)
	{
		auto& session = m_session_registry.create_session();
		auto miner_connection = std::make_shared<Miner_connection>(m_timer_factory, std::move(connection), session);

	
		session.update_connection(miner_connection);

		return miner_connection->connection_handler();
	};

	m_listen_socket->listen(socket_handler);
	
}

void Pool_manager::stop()
{
	m_listen_socket->stop_listen();
}


}