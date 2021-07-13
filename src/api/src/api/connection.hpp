#ifndef NEXUSPOOL_API_CONNECTION_HPP
#define NEXUSPOOL_API_CONNECTION_HPP

#include "network/connection.hpp"
#include <spdlog/spdlog.h>

#include <memory>

namespace nexuspool
{
namespace api
{

class Connection : public std::enable_shared_from_this<Connection>
{
public:

    Connection(network::Connection::Sptr&& connection);

    void stop();

    network::Connection::Handler connection_handler();

private:

    void process_data(network::Shared_payload&& receive_buffer);

    network::Connection::Sptr m_connection;
    std::shared_ptr<spdlog::logger> m_logger;
    std::string m_remote_address;
};

}
}

#endif
