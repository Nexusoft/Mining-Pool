#ifndef NEXUSPOOL_MINER_CONNECTION_IMPL_HPP
#define NEXUSPOOL_MINER_CONNECTION_IMPL_HPP

#include "pool/miner_connection.hpp"
#include "LLP/packet.hpp"
#include <memory>
#include <atomic>

namespace nexuspool
{
class Pool_manager;

class Miner_connection_impl : public Miner_connection, public std::enable_shared_from_this<Miner_connection_impl>
{
public:

    Miner_connection_impl(std::shared_ptr<spdlog::logger> logger,
        network::Connection::Sptr&& connection,
        std::weak_ptr<Pool_manager> pool_manager,
        Session_key session_key,
        Session_registry::Sptr session_registry);

    void stop() override;

    network::Connection::Handler connection_handler() override;
    void set_current_height(std::uint32_t height) override;
    void get_hashrate() override;

private:

    void process_data(network::Shared_payload&& receive_buffer);

    // checks if a new account should be created, add share for session
    void process_accepted();

    bool process_login(Packet login_packet, std::shared_ptr<Session> session);

    std::shared_ptr<spdlog::logger> m_logger;
    network::Connection::Sptr m_connection;
    std::weak_ptr<Pool_manager> m_pool_manager;
    std::atomic<std::uint32_t> m_current_height;
    Session_key m_session_key;
    Session_registry::Sptr m_session_registry;
    std::uint32_t m_pool_nbits;
};

}

#endif
