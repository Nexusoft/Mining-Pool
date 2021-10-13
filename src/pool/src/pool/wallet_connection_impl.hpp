#ifndef NEXUSPOOL_WALLET_CONNECTION_IMPL_HPP
#define NEXUSPOOL_WALLET_CONNECTION_IMPL_HPP

#include "network/connection.hpp"
#include "network/socket.hpp"
#include "network/types.hpp"
#include "chrono/timer_factory.hpp"
#include "pool/timer_manager_wallet.hpp"
#include "pool/wallet_connection.hpp"
#include "block.hpp"
#include "pool/types.hpp"
#include "common/types.hpp"

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>

namespace asio { class io_context; }
namespace spdlog { class logger; }

namespace nexuspool
{
class Pool_manager;

class Wallet_connection_impl : public Wallet_connection, public std::enable_shared_from_this<Wallet_connection_impl>
{
public:

    Wallet_connection_impl(std::shared_ptr<asio::io_context> io_context,
        std::shared_ptr<spdlog::logger> logger,
        std::weak_ptr<Pool_manager> pool_manager,
        common::Mining_mode mining_mode,
        std::uint16_t connection_retry_interval,
        std::uint16_t get_height_interval,
        chrono::Timer_factory::Sptr timer_factory,
        network::Socket::Sptr socket);

    bool connect(network::Endpoint const& wallet_endpoint) override;

    // Close connection
    void stop() override;

    void submit_block(network::Shared_payload&& block_data, std::uint32_t block_map_id, Submit_block_handler&& handler) override;
    void get_block(Get_block_handler&& handler) override;

private:

    void process_data(network::Shared_payload&& receive_buffer);

    void retry_connect(network::Endpoint const& wallet_endpoint);

    std::shared_ptr<::asio::io_context> m_io_context;
    std::shared_ptr<spdlog::logger> m_logger;
    std::weak_ptr<Pool_manager> m_pool_manager;
    common::Mining_mode m_mining_mode;
    std::uint16_t const m_connection_retry_interval;
    std::uint16_t const m_get_height_interval;
    network::Socket::Sptr m_socket;
    network::Connection::Sptr m_connection;
    Timer_manager_wallet m_timer_manager;
    std::uint32_t m_current_height;

    // get_block variables
    std::mutex m_get_block_mutex;
    std::atomic<bool> m_get_block_pool_manager;
    std::queue<Get_block_handler> m_pending_get_block_handlers;

    // submit_block variables
    std::mutex m_submit_block_mutex;
    std::queue<std::pair<std::uint32_t, Submit_block_handler>> m_pending_submit_block_handlers;
};
}

#endif
