#ifndef NEXUSPOOL_WALLET_CONNECTION_HPP
#define NEXUSPOOL_WALLET_CONNECTION_HPP

#include "network/connection.hpp"
#include "network/socket.hpp"
#include "network/types.hpp"
#include <spdlog/spdlog.h>
#include "chrono/timer_factory.hpp"
#include "timer_manager.hpp"
#include "block.hpp"
#include "types.hpp"
#include "common/types.hpp"

#include <memory>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>

namespace asio { class io_context; }

namespace nexuspool
{
namespace config { class Config; }
class Pool_manager;

class Wallet_connection : public std::enable_shared_from_this<Wallet_connection>
{
public:

    Wallet_connection(std::shared_ptr<asio::io_context> io_context,
        std::weak_ptr<Pool_manager> pool_manager,
        common::Mining_mode mining_mode,
        config::Config& config,
        chrono::Timer_factory::Sptr timer_factory, 
        network::Socket::Sptr socket);

    bool connect(network::Endpoint const& wallet_endpoint);

    // Close connection
    void stop();

    void submit_block(std::vector<std::uint8_t> const& block_data, std::vector<std::uint8_t> const& nonce);
    void get_block(Get_block_handler&& handler);

private:

    void process_data(network::Shared_payload&& receive_buffer);

    void retry_connect(network::Endpoint const& wallet_endpoint);

    std::shared_ptr<::asio::io_context> m_io_context;
    std::weak_ptr<Pool_manager> m_pool_manager;
    config::Config& m_config;
    common::Mining_mode m_mining_mode;
    network::Socket::Sptr m_socket;
    network::Connection::Sptr m_connection;
    std::shared_ptr<spdlog::logger> m_logger;
    Timer_manager m_timer_manager;
    std::uint32_t m_current_height;

    // get_block variables
    std::mutex m_get_block_mutex;
    std::atomic<bool> m_get_block_pool_manager;
    std::queue<Get_block_handler> m_pending_get_block_handlers;
};
}

#endif
