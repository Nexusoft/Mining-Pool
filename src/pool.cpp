#include "pool.hpp"
#include "network/create_component.hpp"
#include "network/component.hpp"
#include "config/validator.hpp"

#include "chrono/timer_factory.hpp"

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

		// network initialisation
		m_network_component = network::create_component(m_io_context);
		auto socket_factory = m_network_component->get_socket_factory();

		network::Endpoint listen_endpoint{ network::Transport_protocol::tcp, "127.0.0.1", m_config.get_local_port() };
		m_listen_socket = socket_factory->create_socket(listen_endpoint);

		return true;
	}

	void Pool::run()
	{
		// connect daemon to wallet	
		if (!m_daemon_connection->connect(network::Endpoint{ network::Transport_protocol::tcp, m_config.get_wallet_ip(), m_config.get_port() }))
		{
			m_logger->critical("Couldn't connect to wallet using ip {} and port {}", m_config.get_wallet_ip(), m_config.get_wallet_port());
			return;
		}


		// on listen/accept, save created connection to pool_conenctions and call the connection_handler of created pool connection object
		auto socket_handler = [this](network::Connection::Sptr&& connection)
		{
			//auto pool_connection = std::make_shared<Pool_connection>(std::move(connection), m_daemon_connection, m_data_registry);

			//std::lock_guard<std::mutex> lk(m_pool_connections_mutex);
			//m_pool_connections.push_back(pool_connection);
			//return pool_connection->init_connection_handler();
			return network::Connection::Handler{};
		};

		m_listen_socket->listen(socket_handler);

		m_io_context->run();
	}
}