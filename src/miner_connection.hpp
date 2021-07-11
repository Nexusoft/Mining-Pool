#ifndef NEXUSPOOL_MINER_CONNECTION_HPP
#define NEXUSPOOL_MINER_CONNECTION_HPP

#include "network/connection.hpp"
#include <spdlog/spdlog.h>
#include "chrono/timer_factory.hpp"
#include "timer_manager.hpp"

#include <memory>
#include <string>

namespace nexuspool
{
class Session;

class Miner_connection : public std::enable_shared_from_this<Miner_connection>
{
public:

    Miner_connection(chrono::Timer_factory::Sptr timer_factory, network::Connection::Sptr&& connection, Session& session);

    void stop();

    network::Connection::Handler connection_handler();

private:

    void process_data(network::Shared_payload&& receive_buffer);

    network::Connection::Sptr m_connection;
    std::shared_ptr<spdlog::logger> m_logger;
    Timer_manager m_timer_manager;
    std::uint32_t m_current_height;
    std::string m_remote_address;
    bool m_connection_closed;	// indicator for server if the network connection has been closed
    Session& m_session;
};
}

#endif
