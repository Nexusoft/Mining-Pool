#ifndef NEXUSPOOL_MINER_CONNECTION_LEGACY_IMPL_HPP
#define NEXUSPOOL_MINER_CONNECTION_LEGACY_IMPL_HPP

#include "pool/miner_connection.hpp"
#include "LLP/packet.hpp"
#include "chrono/timer.hpp"
#include <memory>
#include <atomic>

namespace nexuspool
{
class Pool_manager;

class Miner_connection_legacy_impl : public Miner_connection, public std::enable_shared_from_this<Miner_connection_legacy_impl>
{
public:

    Miner_connection_legacy_impl(std::shared_ptr<spdlog::logger> logger,
        network::Connection::Sptr&& connection,
        std::weak_ptr<Pool_manager> pool_manager,
        Session_key session_key,
        Session_registry::Sptr session_registry,
        chrono::Timer::Uptr get_block_timer);

    void stop() override;
    void send_work(LLP::CBlock const& block) override {}        // not supported
    network::Connection::Handler connection_handler() override;
    void get_hashrate() override {} // not supported
    void send_pool_notification(std::string message) override {} // not supported

private:

    void process_data(network::Shared_payload&& receive_buffer);

    // checks if a new account should be created, add share for session
    void process_accepted();
    void process_login(Packet login_packet, std::shared_ptr<Session> session);

    void get_block(std::shared_ptr<Pool_manager> pool_manager);
    chrono::Timer::Handler get_block_handler(std::uint16_t get_block_interval);

    std::shared_ptr<spdlog::logger> m_logger;
    network::Connection::Sptr m_connection;
    std::weak_ptr<Pool_manager> m_pool_manager;
    Session_key m_session_key;
    Session_registry::Sptr m_session_registry;
    chrono::Timer::Uptr m_get_block_timer;
    std::uint32_t m_pool_nbits;
    std::atomic<std::uint32_t> m_current_height;
};

}

#endif
