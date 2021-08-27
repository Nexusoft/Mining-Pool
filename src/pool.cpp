#include "pool.hpp"
#include "network/create_component.hpp"
#include "network/component.hpp"
#include "persistance/create_component.hpp"
#include "config/validator.hpp"
#include "api/server.hpp"
#include "pool_manager.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <chrono>
#include <fstream>

namespace nexuspool
{

	Pool::Pool()
		: m_io_context{ std::make_shared<::asio::io_context>() }
		, m_signals{ std::make_shared<::asio::signal_set>(*m_io_context) }
	{
		m_logger = spdlog::stdout_color_mt("logger");
		m_logger->set_level(spdlog::level::debug);
		m_logger->set_pattern("[%D %H:%M:%S.%e][%^%l%$] %v");
		// Register to handle the signals that indicate when the server should exit.
	// It is safe to register for the same signal multiple times in a program,
	// provided all registration for the specified signal is made through Asio.
		m_signals->add(SIGINT);
		m_signals->add(SIGTERM);
#if defined(SIGQUIT)
		m_signals->add(SIGQUIT);
#endif // defined(SIGQUIT)

		m_signals->async_wait([this](auto, auto)
		{
			m_logger->info("Shutting down NexusPool");
			m_api_server->stop();
			m_pool_manager->stop();
			m_io_context->stop();
			exit(1);
		});
	}

	Pool::~Pool()
	{
		m_io_context->stop();
	}

	bool Pool::check_config(std::string const& pool_config_file)
	{
		m_logger->info("Running config check for {}", pool_config_file);
		std::ifstream config(pool_config_file);
		if (!config.is_open())
		{
			m_logger->critical("Unable to read {}", pool_config_file);
			return false;
		}

		config::Validator validator{};
		auto result = validator.check(pool_config_file);
		result ? m_logger->info(validator.get_check_result()) : m_logger->error(validator.get_check_result());
		return result;
	}

	bool Pool::init(std::string const& pool_config_file)
	{
		if (!m_config.read_config(pool_config_file))
		{
			return false;
		}

		// data storage initialisation
		m_persistance_component = persistance::create_component(m_logger, m_config.get_persistance_config());

		auto const config_data = storage_config_check();

		// network initialisation
		m_network_component = network::create_component(m_io_context);
		m_pool_manager = std::make_shared<Pool_manager>(m_io_context, 
			m_config, 
			config_data,
			m_network_component->get_socket_factory(), 
			m_persistance_component->get_data_writer_factory(),
			m_persistance_component->get_data_reader_factory());
		m_api_server = std::make_unique<api::Server>(m_logger, m_persistance_component->get_data_reader_factory()->create_data_reader(),
			m_config.get_local_ip(), m_config.get_api_listen_port(), m_network_component->get_socket_factory());

		return true;
	}

	void Pool::run()
	{
		m_api_server->start();
		m_pool_manager->start();
		m_io_context->run();
	}

	persistance::Config_data Pool::storage_config_check()
	{
		persistance::Config_data config_data{};
		auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
		auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();

		config_data = data_reader->get_config();
		std::string const mining_mode{ m_config.get_mining_mode() == config::Mining_mode::HASH ? "HASH" : "PRIME" };
		if (config_data.m_version.empty())
		{
			// No config present
			data_writer->create_config(mining_mode, m_config.get_pool_config().m_fee, m_config.get_pool_config().m_difficulty_divider);
		}
		else
		{
			if (mining_mode != config_data.m_mining_mode &&
				m_config.get_pool_config().m_fee != config_data.m_fee &&
				m_config.get_pool_config().m_difficulty_divider != config_data.m_difficulty_divider)
			{
				// update config but only if there is no round active
				auto const round_data = data_reader->get_latest_round();
				if (round_data.m_is_active)
				{
					m_logger->warn("Pool config can't be changed now. Round {} is active until {}.", round_data.m_round, round_data.m_end_date_time);
				}
				else
				{
					data_writer->update_config(mining_mode, m_config.get_pool_config().m_fee, m_config.get_pool_config().m_difficulty_divider);
				}
			}
		}

		// Get the latest (maybe updated) config from storage
		config_data = data_reader->get_config();
		return config_data;
	}
}