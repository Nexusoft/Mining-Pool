#ifndef NEXUSPOOL_API_SERVER_HPP
#define NEXUSPOOL_API_SERVER_HPP

#include "network/socket_factory.hpp"
#include "network/types.hpp"
#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

namespace nexuspool
{
namespace config { class Config; }
namespace api
{
    class Connection;

class Server 
{
public:

    Server(config::Config& config, network::Socket_factory::Sptr socket_factory);

    void start();
    void stop();

private:

    config::Config& m_config;
    network::Socket_factory::Sptr m_socket_factory;
    std::shared_ptr<spdlog::logger> m_logger;
    network::Socket::Sptr m_listen_socket;
    std::vector<std::shared_ptr<api::Connection>> m_api_connections;
};

}
}

#endif
