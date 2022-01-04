#include "pool.hpp"
#include "network/create_component.hpp"
#include "network/component.hpp"
#include "chrono/create_component.hpp"
#include "persistance/create_component.hpp"
#include "config/validator.hpp"
#include "common/types.hpp"
#include "api/server.hpp"
#include "pool/pool_manager.hpp"
#include "common/pool_api_data_exchange.hpp"
#include "version.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace nexuspool
{

	Pool::Pool()
		: m_io_context{ std::make_shared<::asio::io_context>() }
		, m_signals{ std::make_shared<::asio::signal_set>(*m_io_context) }
	{
		m_config = config::create_config();
		m_api_config = config::create_api_config();
		m_logger = spdlog::stdout_color_mt("logger");
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
			stop();
			exit(1);
		});
	}

	Pool::~Pool()
	{
		stop();
	}

	void Pool::stop()
	{
		if (m_api_server)
		{
			m_api_server->stop();
		}
		m_pool_manager->stop();
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

	bool Pool::init(std::string pool_config_file, std::string api_config_file)
	{
		if (!m_config->read_config(pool_config_file))
		{
			return false;
		}
		// print header
		std::cout << "NexusPool Version " << NexusPool_VERSION_MAJOR << "." << NexusPool_VERSION_MINOR << "\n" << std::endl;

		// logger settings
		if (!m_config->get_logfile().empty())
		{
			// current time as string
			auto t = std::time(nullptr);
			auto tm = *std::localtime(&t);
			std::ostringstream oss;
			oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");

			// add timestamp to logfilename
			auto const& full_logname = m_config->get_logfile();
			std::size_t lastindex = full_logname.find_last_of(".");
			std::string raw_logname = full_logname.substr(0, lastindex);
			raw_logname += "_" + oss.str() + ".log";

			// initialise a new logger
			spdlog::drop("logger");
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(raw_logname, true);

			m_logger = std::make_shared<spdlog::logger>(spdlog::logger("logger", { console_sink, file_sink }));
			m_logger->set_pattern("[%D %H:%M:%S.%e][%^%l%$] %v");
			spdlog::set_default_logger(m_logger);
			spdlog::flush_on(spdlog::level::info);
		}
		m_logger->set_level(static_cast<spdlog::level::level_enum>(m_config->get_log_level()));

		m_timer_component = chrono::create_component(m_io_context);
		m_pool_api_data_exchange = common::create_pool_api_data_exchange();

		// data storage initialisation
		m_persistance_component = persistance::create_component(m_logger, m_config->get_persistance_config());

		// network initialisation
		m_network_component = network::create_component(m_io_context);
		m_pool_manager = create_pool_manager(m_io_context, 
			m_logger,
			m_config,
			m_timer_component->get_timer_factory(),
			m_network_component->get_socket_factory(), 
			m_persistance_component->get_data_writer_factory(),
			m_persistance_component->get_data_reader_factory(),
			m_pool_api_data_exchange);

		if (m_api_config->read_config(api_config_file))
		{
			m_api_server = std::make_unique<api::Server>(m_logger, 
				m_persistance_component->get_data_reader_factory()->create_data_reader(), 
				m_api_config, 
				m_pool_api_data_exchange, 
				m_timer_component->get_timer_factory());
		}
		else
		{
			m_logger->info("No API support");
		}

		return true;
	}

	void Pool::run()
	{
		if (m_api_server)
		{
			m_api_server->start();
		}

		m_pool_manager->start();
		m_io_context->run();
	}
}