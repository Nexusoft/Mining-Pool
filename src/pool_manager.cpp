#include "pool_manager.hpp"
#include "wallet_connection.hpp"
#include "miner_connection.hpp"
#include "config/config.hpp"
#include "reward/create_component.hpp"

namespace nexuspool
{

Pool_manager::Pool_manager(std::shared_ptr<asio::io_context> io_context, 
	config::Config& config,
	network::Socket_factory::Sptr socket_factory,
	persistance::Data_writer_factory::Sptr data_writer_factory,
	persistance::Data_reader_factory::Sptr data_reader_factory)
	: m_io_context{std::move(io_context) }
	, m_config{config}
	, m_timer_factory{std::make_shared<chrono::Timer_factory>(m_io_context)}
	, m_socket_factory{std::move(socket_factory)}
	, m_logger{ spdlog::get("logger") }
	, m_data_writer_factory{std::move(data_writer_factory)}
	, m_data_reader_factory{std::move(data_reader_factory)}
	, m_reward_component{reward::create_component(m_data_writer_factory->create_shared_data_writer(), m_data_reader_factory->create_data_reader())}
	, m_reward_manager{m_reward_component->create_reward_manager()}
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

void Pool_manager::set_current_height(std::uint32_t height) 
{ 
	if (height > m_current_height)
	{
		m_current_height = height;
		// give the miners the new height
		m_session_registry.update_height(m_current_height);
	}
}

void Pool_manager::set_block(LLP::CBlock const& block)
{
	std::scoped_lock(m_block_mutex);
	m_block = block;

}

void Pool_manager::get_block(Get_block_handler&& handler)
{
	m_wallet_connection->get_block(std::move(handler));
}

void Pool_manager::submit_block(LLP::CBlock&& block, std::uint64_t nonce, Submit_block_handler handler)
{
	auto difficulty_result = m_reward_manager->check_difficulty(block, m_pool_nBits);
	switch (difficulty_result)
	{
	case reward::Difficulty_result::accept:
		// record share for this miner connection but don't submit block to wallet
		handler(Submit_block_result::accept);
		break;
	case reward::Difficulty_result::block_found:
		// submit the block to wallet
		// m_wallet_connection->submit_block(block_data, nonce, std::move(handler));
		break;
	case reward::Difficulty_result::reject:
		handler(Submit_block_result::reject);
		break;
	}	
}

std::uint32_t Pool_manager::get_pool_nbits()
{
	return m_pool_nBits;
}

//pool nbits determines the difficulty for the pool.  
//For the hash channel, we set the difficulty to be a divided down version of the main net difficulty
void Pool_manager::set_pool_nbits(std::uint32_t nbits)
{
	uint1024_t target, pool_target;
	target.SetCompact(nbits);
	pool_target = target >> m_config.get_pool_config().m_difficulty_divider;
	m_pool_nBits = pool_target.GetCompact();
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