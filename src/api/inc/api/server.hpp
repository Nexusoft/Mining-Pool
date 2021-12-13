#ifndef NEXUSPOOL_API_SERVER_HPP
#define NEXUSPOOL_API_SERVER_HPP

#include "network/types.hpp"
#include "network/endpoint.hpp"
#include "persistance/data_reader.hpp"
#include "common/pool_api_data_exchange.hpp"

#include <memory>
#include <vector>
#include <string>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{
    class Connection;
    class Shared_data_reader;

class Server 
{
public:

    Server(std::shared_ptr<spdlog::logger> logger,
        persistance::Data_reader::Uptr data_reader,
        std::string public_ip, 
        std::uint16_t api_listen_port,
        common::Pool_api_data_exchange::Sptr pool_api_data_exchange);

    void start();
    void stop();
    network::Endpoint const& local_endpoint() const { return network::Endpoint{}; }

private:

    std::shared_ptr<spdlog::logger> m_logger;
    std::shared_ptr<Shared_data_reader> m_shared_data_reader;
    std::string m_public_ip;
    std::uint16_t m_api_listen_port;
    network::Endpoint m_local_endpoint;
    common::Pool_api_data_exchange::Sptr m_pool_api_data_exchange;
};

}
}

#endif
