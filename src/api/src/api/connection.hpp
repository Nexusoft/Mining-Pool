#ifndef NEXUSPOOL_API_CONNECTION_HPP
#define NEXUSPOOL_API_CONNECTION_HPP

#include "network/connection.hpp"
#include "api/parser.hpp"

#include <memory>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{

class Connection : public std::enable_shared_from_this<Connection>
{
public:

    Connection(std::shared_ptr<spdlog::logger> logger, network::Connection::Sptr&& connection, Parser::Sptr parser);

    void stop();

    network::Connection::Handler connection_handler();

private:

    void process_data(network::Shared_payload&& receive_buffer);

    network::Connection::Sptr m_connection;
    std::shared_ptr<spdlog::logger> m_logger;
    Parser::Sptr m_parser;
};

}
}

#endif
