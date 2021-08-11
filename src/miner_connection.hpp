#ifndef NEXUSPOOL_MINER_CONNECTION_HPP
#define NEXUSPOOL_MINER_CONNECTION_HPP

#include "network/connection.hpp"
#include <spdlog/spdlog.h>
#include "chrono/timer_factory.hpp"
#include "timer_manager.hpp"
#include "session.hpp"

#include <memory>
#include <string>
#include <atomic>

namespace nexuspool
{
class Pool_manager;

class Miner_connection : public std::enable_shared_from_this<Miner_connection>
{
public:

    Miner_connection(chrono::Timer_factory::Sptr timer_factory, 
        network::Connection::Sptr&& connection, 
        std::weak_ptr<Pool_manager> pool_manager,
        Session_key session_key,
        Session_registry& session_registry);

    void stop();

    network::Connection::Handler connection_handler();
    void set_current_height(std::uint32_t height);

private:

    void process_data(network::Shared_payload&& receive_buffer);

    // checks if a new account should be created -> create it if necessary
    void create_new_account();

    network::Connection::Sptr m_connection;
    std::weak_ptr<Pool_manager> m_pool_manager;
    std::shared_ptr<spdlog::logger> m_logger;
    Timer_manager m_timer_manager;
    std::atomic<std::uint32_t> m_current_height;
    std::string m_remote_address;
    bool m_connection_closed;	// indicator for server if the network connection has been closed
    Session_key m_session_key;
    Session_registry& m_session_registry;
};
}

#endif
