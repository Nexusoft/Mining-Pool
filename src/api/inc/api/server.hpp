#ifndef NEXUSPOOL_API_SERVER_HPP
#define NEXUSPOOL_API_SERVER_HPP

#include "network/socket_factory.hpp"
#include "network/types.hpp"
#include "persistance/data_storage.hpp"
#include <spdlog/spdlog.h>

#include <memory>
#include <vector>
#include <string>

namespace nexuspool
{
namespace api
{
    class Connection;

class Server 
{
public:

    Server(persistance::Data_storage::Sptr data_storage, 
        std::string local_ip, 
        std::uint16_t api_listen_port, 
        network::Socket_factory::Sptr socket_factory);

    void start();
    void stop();

private:

    persistance::Data_storage::Sptr m_data_storage;
    std::string m_local_ip;
    std::uint16_t m_api_listen_port;
    network::Socket_factory::Sptr m_socket_factory;
    std::shared_ptr<spdlog::logger> m_logger;
    network::Socket::Sptr m_listen_socket;
    std::vector<std::shared_ptr<api::Connection>> m_api_connections;
};

}
}

#endif
