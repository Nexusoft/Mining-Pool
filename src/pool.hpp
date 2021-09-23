#ifndef NEXUSPOOL_POOL_HPP
#define NEXUSPOOL_POOL_HPP

#include "config/config.hpp"
#include "persistance/types.hpp"
#include <spdlog/spdlog.h>
#include <asio/signal_set.hpp>

#include <memory>
#include <string>

namespace asio { class io_context; }
namespace nexuspool
{
namespace network { class Socket; class Component; }
namespace persistance { class Component; }
namespace chrono { class Timer; }
namespace api { class Server; }
class Pool_manager;

class Pool
{
public:

	Pool();
	~Pool();

	bool init(std::string const& pool_config_file);
	bool check_config(std::string const& pool_config_file);
	void run();

private:

	persistance::Config_data storage_config_check();

	std::unique_ptr<network::Component> m_network_component;
	std::unique_ptr<persistance::Component> m_persistance_component;
	std::shared_ptr<network::Socket> m_listen_socket;
	std::shared_ptr<::asio::io_context> m_io_context;
	std::shared_ptr<spdlog::logger> m_logger;
	std::shared_ptr<Pool_manager> m_pool_manager;

	std::shared_ptr<config::Config> m_config;
	std::unique_ptr<api::Server> m_api_server;

	std::shared_ptr<::asio::signal_set> m_signals;
};

}

#endif
