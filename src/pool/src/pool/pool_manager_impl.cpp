#include "pool/pool_manager_impl.hpp"
#include "pool/session_impl.hpp"
#include "pool/wallet_connection_impl.hpp"
#include "pool/miner_connection_impl.hpp"
#include "config/config.hpp"
#include "reward/create_component.hpp"
#include "chrono/create_component.hpp"
#include "LLP/utils.hpp"

namespace nexuspool
{

Pool_manager::Sptr create_pool_manager(std::shared_ptr<asio::io_context> io_context,
	std::shared_ptr<spdlog::logger> logger,
	config::Config::Sptr config,
	chrono::Timer_factory::Sptr timer_factory,
	network::Socket_factory::Sptr socket_factory,
	persistance::Data_writer_factory::Sptr data_writer_factory,
	persistance::Data_reader_factory::Sptr data_reader_factory,
	common::Pool_api_data_exchange::Sptr pool_api_data_exchange)
{
	return std::make_shared<Pool_manager_impl>(std::move(io_context),
		std::move(logger),
		std::move(config),
		std::move(timer_factory),
		std::move(socket_factory),
		std::move(data_writer_factory),
		std::move(data_reader_factory),
		std::move(pool_api_data_exchange));
}


Pool_manager_impl::Pool_manager_impl(std::shared_ptr<asio::io_context> io_context,
	std::shared_ptr<spdlog::logger> logger,
	config::Config::Sptr config,
	chrono::Timer_factory::Sptr timer_factory,
	network::Socket_factory::Sptr socket_factory,
	persistance::Data_writer_factory::Sptr data_writer_factory,
	persistance::Data_reader_factory::Sptr data_reader_factory,
	common::Pool_api_data_exchange::Sptr pool_api_data_exchange)
	: m_io_context{std::move(io_context) }
	, m_logger{ std::move(logger)}
	, m_config{std::move(config)}
	, m_timer_factory{ std::move(timer_factory) }
	, m_socket_factory{std::move(socket_factory)}
	, m_data_writer_factory{std::move(data_writer_factory)}
	, m_data_reader_factory{std::move(data_reader_factory)}
	, m_pool_api_data_exchange{std::move(pool_api_data_exchange)}
	, m_http_component{ nexus_http_interface::create_component(m_logger, m_config->get_wallet_ip())}
	, m_reward_component{reward::create_component(m_logger, 
		m_http_component,
		m_data_writer_factory->create_shared_data_writer(), 
		m_data_reader_factory->create_data_reader(),
		m_config->get_pool_config().m_account,
		m_config->get_pool_config().m_pin,
		m_config->get_pool_config().m_fee)}
	, m_listen_socket{}
	, m_session_registry{std::make_shared<Session_registry_impl>(
		m_data_reader_factory->create_data_reader(), 
		m_data_writer_factory->create_shared_data_writer(), 
		m_http_component, 
		m_config->get_session_expiry_time(),
		m_config->get_mining_mode())}
	, m_total_blocks{0}
	, m_total_shares{0}
	, m_current_height{0}
	, m_block_map_id{0}
{
	m_session_registry_maintenance = m_timer_factory->create_timer();
	m_end_round_timer = m_timer_factory->create_timer();
	m_update_block_hashes_timer = m_timer_factory->create_timer();
}

void Pool_manager_impl::start()
{
	m_storage_config_data = storage_config_check();
	network::Endpoint const wallet_endpoint{ network::Transport_protocol::tcp, m_config->get_wallet_ip(), m_config->get_wallet_port() };
	network::Endpoint const local_endpoint{ network::Transport_protocol::tcp, m_config->get_local_ip(), m_config->get_local_port() };
	common::Mining_mode const mining_mode = m_storage_config_data.m_mining_mode == "HASH" ? common::Mining_mode::HASH : common::Mining_mode::PRIME;

	auto self = shared_from_this();
	// connect to wallet
	m_wallet_connection = std::make_shared<Wallet_connection_impl>(
		m_io_context, 
		m_logger, 
		self, 
		mining_mode, 
		m_config->get_connection_retry_interval(), 
		m_config->get_height_interval(), 
		m_timer_factory, 
		m_socket_factory->create_socket(local_endpoint));
	if (!m_wallet_connection->connect(wallet_endpoint))
	{
		m_logger->critical("Couldn't connect to wallet using ip {} and port {}", m_config->get_wallet_ip(), m_config->get_wallet_port());
		return;
	}

	// check if there is an active round -> if not start one
	if (!m_reward_component->is_round_active())
	{
		if (!m_reward_component->start_round(m_storage_config_data.m_round_duration_hours))
		{
			// error
			return;
		}
	}

	// calculate round duration and start timer for end_round
	std::chrono::system_clock::time_point round_start_time, round_end_time;
	m_reward_component->get_start_end_round_times(round_start_time, round_end_time);
	auto const time_now = std::chrono::system_clock::now();
	if (time_now > round_end_time)
	{
		end_round();
	}
	else
	{
		// start timer for end_round
		m_end_round_timer->start(chrono::Seconds(std::chrono::duration_cast<std::chrono::seconds>(round_end_time - time_now).count()), end_round_handler());
	}

	// On startup check if there are still unpaid rounds
	m_reward_component->process_unpaid_rounds();

	// listen to connecting miners
	network::Endpoint local_listen_endpoint{ network::Transport_protocol::tcp, m_config->get_public_ip(), m_config->get_miner_listen_port() };
	m_listen_socket = m_socket_factory->create_socket(local_listen_endpoint);

	
	// on listen/accept, save created connection to pool_conenctions and call the connection_handler of created pool connection object
	auto socket_handler = [self](network::Connection::Sptr&& connection)
	{
		auto const session_key = self->m_session_registry->create_session();
		auto miner_connection = create_miner_connection(self->m_logger, std::move(connection), self, session_key, self->m_session_registry);

		auto session = self->m_session_registry->get_session(session_key);
		session->update_connection(miner_connection);
		session->set_update_time(std::chrono::steady_clock::now());

		return miner_connection->connection_handler();
	};

	m_listen_socket->listen(socket_handler);

	m_session_registry_maintenance->start(chrono::Seconds(m_config->get_session_expiry_time()), 
		session_registry_maintenance_handler(m_config->get_session_expiry_time()));

	//m_update_block_hashes_timer->start(chrono::Seconds(5*60), update_block_hashes_handler(5*60));
	
}

void Pool_manager_impl::stop()
{
	m_session_registry_maintenance->stop();
	m_end_round_timer->stop();
	//m_update_block_hashes_timer->stop();
	m_session_registry->stop();	// clear sessions and deletes miner_connection objects
	m_wallet_connection->stop();
	m_listen_socket->stop_listen();
}

void Pool_manager_impl::set_current_height(std::uint32_t height)
{ 
	if (height > m_current_height)
	{
		m_current_height = height;
		// new block -> clear pending blocks from previous height
		m_block_map.clear();
		m_block_map_id = 0;
	}

	// give the miners the height
	m_session_registry->update_height(m_current_height);
}

void Pool_manager_impl::set_block(LLP::CBlock const& block)
{
	std::scoped_lock(m_block_mutex);
	m_block = block;

	//pool nbits determines the difficulty for the pool.  
	//For the hash channel, we set the difficulty to be a divided down version of the main net difficulty.
	if (block.nChannel == 2) //hash channel
	{
		uint1024_t target, pool_target;
		target.SetCompact(m_block.nBits);
		if (m_storage_config_data.m_difficulty_divider >= 0)
			pool_target = target << m_storage_config_data.m_difficulty_divider;
		else
			pool_target = target;
		m_pool_nBits = pool_target.GetCompact();
	}
	else if (block.nChannel == 1) //prime channel
	{
		double difficulty = get_difficulty(m_block.nBits, block.nChannel);
		difficulty -= m_storage_config_data.m_difficulty_divider;
		difficulty = std::max(2.0, difficulty);
		m_pool_nBits = static_cast<uint32_t>(difficulty * 10000000.0);
	}
	else
	{
		m_pool_nBits = block.nBits;
	}

}

void Pool_manager_impl::add_block_to_storage(std::uint32_t block_map_id)
{
	auto submit_block_data = m_block_map[block_map_id];
	auto const block_hash = submit_block_data.m_block->GetHash().ToString();
	auto data_writer = m_data_writer_factory->create_shared_data_writer();
	persistance::Block_data block_data;
	block_data.m_height = submit_block_data.m_block->nHeight;
	block_data.m_type = submit_block_data.m_block->nChannel == 1 ? "prime" : "hash";
	block_data.m_orphan = 0;
	block_data.m_block_finder = submit_block_data.m_blockfinder;
	block_data.m_difficulty = get_difficulty(submit_block_data.m_block->nBits, submit_block_data.m_block->nChannel);
	block_data.m_round = m_reward_component->get_current_round();
	data_writer->add_block(std::move(block_data));

	m_reward_component->block_found();
	m_total_blocks++;
	m_total_shares++;
}

void Pool_manager_impl::get_block(Get_block_handler&& handler)
{
	m_wallet_connection->get_block(std::move(handler));
}

void Pool_manager_impl::submit_block(std::unique_ptr<LLP::CBlock> block, Session_key miner_key, Submit_block_handler handler)
{
	auto session = m_session_registry->get_session(miner_key);
	if (m_total_blocks > 0)
	{
		session->update_hashrate(m_pool_nBits, block->nBits, static_cast<double>(m_total_shares) / static_cast<double>(m_total_blocks));
	}

	auto difficulty_result = m_reward_component->check_difficulty(*block, m_pool_nBits);
	switch (difficulty_result)
	{
	case reward::Difficulty_result::accept:
	{
		// record share for this miner connection but don't submit block to wallet
		handler(Submit_block_result::accept);
		m_total_shares++;
		break;
	}
	case reward::Difficulty_result::block_found:
	{
		// submit the block to wallet
		auto nonce = nexuspool::uint2bytes64(block->nNonce);
		auto block_data = std::make_shared<std::vector<std::uint8_t>>(block->hashMerkleRoot.GetBytes());
		block_data->insert(block_data->end(), nonce.begin(), nonce.end());
		m_block_map.emplace(std::make_pair(m_block_map_id.load(), Submit_block_data{ std::move(block), session->get_user_data().m_account.m_address }));
		m_wallet_connection->submit_block(std::move(block_data), m_block_map_id, std::move(handler));
		m_block_map_id++;
		break;
	}
	case reward::Difficulty_result::reject:
		handler(Submit_block_result::reject);
		break;
	}
}

std::uint32_t Pool_manager_impl::get_pool_nbits() const
{
	std::scoped_lock(m_block_mutex);
	return m_pool_nBits;
}

chrono::Timer::Handler Pool_manager_impl::update_block_hashes_handler(std::uint16_t update_block_hashes_interval)
{
	return[this, update_block_hashes_interval]()
	{
		m_reward_component->update_block_hashes_from_current_round();

		// restart timer
		m_update_block_hashes_timer->start(chrono::Seconds(update_block_hashes_interval),
			update_block_hashes_handler(update_block_hashes_interval));
	};
}

chrono::Timer::Handler Pool_manager_impl::session_registry_maintenance_handler(std::uint16_t session_registry_maintenance_interval)
{
	return[this, session_registry_maintenance_interval]()
	{
		m_session_registry->clear_unused_sessions();
		m_pool_api_data_exchange->set_active_miners(m_session_registry->get_sessions_size());	// update the currently active miners on pool

		// restart timer
		m_session_registry_maintenance->start(chrono::Seconds(session_registry_maintenance_interval),
			session_registry_maintenance_handler(session_registry_maintenance_interval));
	};
}

chrono::Timer::Handler Pool_manager_impl::end_round_handler()
{
	return[this]()
	{
		end_round();
	};
}

void Pool_manager_impl::end_round()
{
	auto const current_round = m_reward_component->get_current_round();
	m_reward_component->end_round(current_round);
	m_reward_component->pay_round(current_round);
	m_logger->trace("Total_blocks: {}, Total_shares {}", m_total_blocks, m_total_shares);
	m_session_registry->end_round();

	// update config in storage
	m_storage_config_data = storage_config_check();

	// start next round
	if (!m_reward_component->start_round(m_storage_config_data.m_round_duration_hours))
	{
		return;
	}

	// start timer for next end_round
	// calculate round duration and start timer for end_round
	std::chrono::system_clock::time_point round_start_time, round_end_time;
	m_reward_component->get_start_end_round_times(round_start_time, round_end_time);
	auto time_now = std::chrono::system_clock::now();
	m_end_round_timer->start(chrono::Seconds(std::chrono::duration_cast<std::chrono::seconds>(round_end_time - time_now).count()), end_round_handler());
}

persistance::Config_data Pool_manager_impl::storage_config_check()
{
	persistance::Config_data config_data{};
	auto data_writer = m_data_writer_factory->create_shared_data_writer();
	auto data_reader = m_data_reader_factory->create_data_reader();

	config_data = data_reader->get_config();
	std::string const mining_mode{ m_config->get_mining_mode() == common::Mining_mode::HASH ? "HASH" : "PRIME" };
	if (config_data.m_version.empty())
	{
		// No config present
		data_writer->create_config(mining_mode, m_config->get_pool_config().m_fee, m_config->get_pool_config().m_difficulty_divider, m_config->get_pool_config().m_round_duration_hours);
	}
	else
	{
		if (mining_mode != config_data.m_mining_mode ||
			m_config->get_pool_config().m_fee != config_data.m_fee ||
			m_config->get_pool_config().m_difficulty_divider != config_data.m_difficulty_divider ||
			m_config->get_pool_config().m_round_duration_hours != config_data.m_round_duration_hours)
		{
			// update config but only if there is no round active
			auto const round_data = data_reader->get_latest_round();
			if (round_data.m_is_active)
			{
				m_logger->warn("Pool config can't be changed now. Round {} is active until {}.", round_data.m_round, round_data.m_end_date_time);
			}
			else
			{
				data_writer->update_config(mining_mode, m_config->get_pool_config().m_fee, m_config->get_pool_config().m_difficulty_divider, m_config->get_pool_config().m_round_duration_hours);
				// reset total blocks/total shares only if the difficulty has changed
				if (m_config->get_pool_config().m_difficulty_divider != config_data.m_difficulty_divider)
				{
					m_total_blocks = 0;
					m_total_shares = 0;
				}
			}
		}
	}

	// Get the latest (maybe updated) config from storage
	config_data = data_reader->get_config();
	return config_data;
}





}