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
			m_persistance_component->stop();
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
		m_persistance_component = persistance::create_component(m_logger, m_config);
		m_persistance_component->start();
		auto data_storage = m_persistance_component->get_data_storage_factory()->create_data_storage();
		// network initialisation
		m_network_component = network::create_component(m_io_context);
		m_pool_manager = std::make_shared<Pool_manager>(m_io_context, m_config, m_network_component->get_socket_factory());
		m_api_server = std::make_unique<api::Server>(m_logger, data_storage, m_config.get_local_ip(), m_config.get_api_listen_port(), m_network_component->get_socket_factory());

		return true;
	}

	void Pool::run()
	{
		m_api_server->start();
		m_pool_manager->start();
		m_io_context->run();
	}
}