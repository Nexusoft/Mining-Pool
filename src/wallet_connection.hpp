#ifndef NEXUSPOOL_WALLET_CONNECTION_HPP
#define NEXUSPOOL_WALLET_CONNECTION_HPP

#include "network/connection.hpp"
#include "network/socket.hpp"
#include "network/types.hpp"
#include <spdlog/spdlog.h>
#include "chrono/timer_factory.hpp"
#include "timer_manager.hpp"
#include "block.hpp"

#include <memory>
#include <vector>

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
        config::Config& config,
        chrono::Timer_factory::Sptr timer_factory, 
        network::Socket::Sptr socket);

    bool connect(network::Endpoint const& wallet_endpoint);

    // Close connection
    void stop();

    void submit_block(std::vector<std::uint8_t> const& block_data, std::vector<std::uint8_t> const& nonce);
    void get_block();

private:

    void process_data(network::Shared_payload&& receive_buffer);

    // Convert the Header of a Block into a Byte Stream for Reading and Writing Across Sockets
    LLP::CBlock deserialize_block(network::Shared_payload data);

    void retry_connect(network::Endpoint const& wallet_endpoint);

    std::shared_ptr<::asio::io_context> m_io_context;
    std::weak_ptr<Pool_manager> m_pool_manager;
    config::Config& m_config;
    network::Socket::Sptr m_socket;
    network::Connection::Sptr m_connection;
    std::shared_ptr<spdlog::logger> m_logger;
    Timer_manager m_timer_manager;
    std::uint32_t m_current_height;
};
}

#endif
