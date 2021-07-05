#ifndef NEXUSPOOL_POOL_HPP
#define NEXUSPOOL_POOL_HPP

#include "config/config.hpp"
#include "chrono/timer.hpp"
#include <spdlog/spdlog.h>
#include <asio/signal_set.hpp>

#include <memory>
#include <string>

namespace asio {
	class io_context;
}
namespace nexuspool
{
	namespace network { class Socket; class Component; }
	namespace chrono { class Timer; }

	class Pool
	{
	public:

		Pool();
		~Pool();

		bool init(std::string const& pool_config_file);
		bool check_config(std::string const& pool_config_file);
		void run();

	private:

		std::unique_ptr<network::Component> m_network_component;
		std::shared_ptr<network::Socket> m_listen_socket;
		std::shared_ptr<::asio::io_context> m_io_context;
		std::shared_ptr<spdlog::logger> m_logger;

		config::Config m_config;

		std::shared_ptr<::asio::signal_set> m_signals;
	};

}

#endif
