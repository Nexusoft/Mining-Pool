#include "pool/wallet_connection_impl.hpp"
#include "pool/pool_manager.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace nexuspool
{
Wallet_connection_impl::Wallet_connection_impl(std::shared_ptr<asio::io_context> io_context,
    std::shared_ptr<spdlog::logger> logger,
    std::weak_ptr<Pool_manager> pool_manager,
    common::Mining_mode mining_mode,
    std::uint16_t connection_retry_interval, 
    std::uint16_t get_height_interval,
    chrono::Timer_factory::Sptr timer_factory, 
    network::Socket::Sptr socket)
    : m_io_context{ std::move(io_context) }
    , m_logger{std::move(logger)}
    , m_pool_manager{std::move(pool_manager)}
    , m_mining_mode{mining_mode}
    , m_connection_retry_interval{ connection_retry_interval }
    , m_get_height_interval{ get_height_interval }
    , m_socket{ std::move(socket) }
    , m_timer_factory{ std::move(timer_factory) }
    , m_timer_manager{ m_timer_factory }
    , m_current_height{0}
    , m_get_block_pool_manager{false}
    , m_stopped{false}
{
    m_block_resubmit_timer = m_timer_factory->create_timer();
}

void Wallet_connection_impl::stop()
{
    m_stopped = true;
    m_block_resubmit_timer->stop();
    m_timer_manager.stop();
    m_connection->close();
}

void Wallet_connection_impl::retry_connect(network::Endpoint const& wallet_endpoint)
{
    if (m_stopped)
    {
        return;
    }

    m_connection->close();

    // retry connect
    m_logger->info("Connection retry {} seconds", m_connection_retry_interval);
    m_timer_manager.start_connection_retry_timer(m_connection_retry_interval, shared_from_this(), wallet_endpoint);
}

bool Wallet_connection_impl::connect(network::Endpoint const& wallet_endpoint)
{
    std::weak_ptr<Wallet_connection_impl> weak_self = shared_from_this();
    auto connection = m_socket->connect(wallet_endpoint, [weak_self, wallet_endpoint](auto result, auto receive_buffer)
        {
            auto self = weak_self.lock();
            if (self)
            {
                if (result == network::Result::connection_declined ||
                    result == network::Result::connection_aborted ||
                    result == network::Result::connection_closed ||
                    result == network::Result::connection_error)
                {
                    self->m_logger->error("Connection to wallet {} not sucessful. Result: {}", wallet_endpoint.to_string(), network::Result::code_to_string(result));
                    self->retry_connect(wallet_endpoint);
                }
                else if (result == network::Result::connection_ok)
                {
                    self->m_logger->info("Connection to wallet established");

                    Packet packet{ Packet::SET_CHANNEL, uint2bytes(self->m_mining_mode == common::Mining_mode::PRIME ? 1U : 2U) };
                    self->m_connection->transmit(packet.get_bytes());

                    self->m_timer_manager.start_get_height_timer(self->m_get_height_interval, self->m_connection);
                }
                else
                {	
                    if (!self->m_connection)
                    {
                        self->m_logger->error("No connection to wallet.");
                        self->retry_connect(wallet_endpoint);
                    }
                    // data received
                    self->process_data(std::move(receive_buffer));
                }
            }
        });

    if (!connection)
    {
        return false;
    }

    m_connection = std::move(connection);
    return true;
}

void Wallet_connection_impl::process_data(network::Shared_payload&& receive_buffer)
{
    // if we don't have a connection to the wallet we cant do anything useful.
    if (!m_connection)
    {
        return;
    }

    auto remaining_size = receive_buffer->size();
    do
    {
        auto packet = extract_packet_from_buffer(receive_buffer, remaining_size, receive_buffer->size() - remaining_size);
        if (!packet.is_valid())
        {
            // log invalid packet
            m_logger->error("Received packet is invalid. Header: {}", packet.m_header);
            continue;
        }

        if (packet.m_header == Packet::PING)
        {
            m_logger->trace("Ping received");
        }
        else if (packet.m_header == Packet::BLOCK_HEIGHT)
        {
            auto pool_manager_shared = m_pool_manager.lock();
            if (!pool_manager_shared)
                break;

            auto const height = bytes2uint(*packet.m_data);
            if (height > m_current_height)
            {
                m_block_resubmit_timer->stop();
                m_current_height = height;
                m_logger->info("Nexus Network: New Block with height {}", m_current_height);

                // update height at pool_manager
                pool_manager_shared->set_current_height(m_current_height);
                m_get_block_pool_manager = true;

                // clear pending get_block handlers
                {
                    std::scoped_lock lock(m_get_block_mutex);
                    std::queue<Get_block_handler> empty_queue;
                    std::swap(m_pending_get_blocks, empty_queue);
                }

                // get new block from wallet for pool_manager
                Packet packet_get_block{ Packet::GET_BLOCK, nullptr };
                m_connection->transmit(packet_get_block.get_bytes());
            }
            else
            {
                // send the height message to all miners
                pool_manager_shared->set_current_height(m_current_height);
            }
        }
        // Block from wallet received
        else if (packet.m_header == Packet::BLOCK_DATA)
        {
            auto block = LLP::deserialize_block(std::move(*packet.m_data));
            if (block.nHeight == m_current_height)
            {
                if (m_get_block_pool_manager) // pool_manager get_block has priority
                {
                    auto pool_manager_shared = m_pool_manager.lock();
                    if (!pool_manager_shared)
                        break;

                    pool_manager_shared->set_block(std::move(block));
                    m_get_block_pool_manager = false;
                }
                else
                {
                    // get oldest pending_get_block handler from miner_connection and call it then pop()
                    std::scoped_lock lock(m_get_block_mutex);
                    if (!m_pending_get_blocks.empty())
                    {
                        auto handler = m_pending_get_blocks.front();
                        handler(block);
                        m_pending_get_blocks.pop();
                    }
                }
            }
            else if (block.nHeight < m_current_height)
            {
                std::scoped_lock lock(m_get_block_mutex);
                m_logger->trace("Block Obsolete Height = {}, Pending miner blocks = {}", block.nHeight, m_pending_get_blocks.size());
                if (block.nHeight == 256)
                {
                    //request a new block if the wallet sends garbage height
                    Packet packet_get_block{ Packet::GET_BLOCK, nullptr };
                    m_connection->transmit(packet_get_block.get_bytes());
                }
            }
            else
            {
                m_logger->trace("Block Obsolete Height = {}", block.nHeight);
            }
        }
        else if (packet.m_header == Packet::ACCEPT)
        {
            m_block_resubmit_timer->stop();
            m_logger->info("Block Accepted By Nexus Network.");
            auto pool_manager_shared = m_pool_manager.lock();
            if (!pool_manager_shared)
                break;

            // get_height immediately to get the next block faster than waiting on get_height_timer
            Packet packet_get_height{ Packet::GET_HEIGHT, nullptr };
            m_connection->transmit(packet_get_height.get_bytes());

            // the oldest handler is the first one who submitted the block
            std::scoped_lock lock(m_submit_block_mutex);
            auto handler = m_pending_submit_block_handlers.front();
            pool_manager_shared->add_block_to_storage(handler.first);
            handler.second(Submit_block_result::block_found);
            m_pending_submit_block_handlers.pop();
        }
        else if (packet.m_header == Packet::REJECT)
        {
            m_block_resubmit_timer->stop();
            m_logger->warn("Block Rejected by Nexus Network.");

            Packet packet_get_block{ Packet::GET_BLOCK, nullptr };
            m_connection->transmit(packet_get_block.get_bytes());

            std::scoped_lock lock(m_submit_block_mutex);
            auto handler = m_pending_submit_block_handlers.front();
            handler.second(Submit_block_result::reject);
            m_pending_submit_block_handlers.pop();
        }
        else
        {
            m_logger->error("Invalid header received.");
        }
    }
    while (remaining_size != 0);
}

void Wallet_connection_impl::submit_block(network::Shared_payload&& block_data, std::uint32_t block_map_id, Submit_block_handler&& handler)
{
    m_logger->info("Submitting Block...");

    m_submit_block_packet = Packet{ Packet::SUBMIT_BLOCK, std::move(block_data) };
    m_connection->transmit(m_submit_block_packet.get_bytes());

    m_block_resubmit_timer->start(chrono::Seconds(2), block_resubmit_handler(2U));

    // store block request handler in pending list (handler comes from miner_connection)
    std::scoped_lock lock(m_submit_block_mutex);
    m_pending_submit_block_handlers.emplace(std::make_pair(block_map_id, handler));
}

void Wallet_connection_impl::get_block(Get_block_handler&& handler)
{
    if (!m_connection)
    {
        return;
    }

    Packet packet_get_block{ Packet::GET_BLOCK, nullptr };
    m_connection->transmit(packet_get_block.get_bytes());

    // store block request handler in pending list (handler comes from miner_connection)
    std::scoped_lock lock(m_get_block_mutex);
    m_pending_get_blocks.emplace(std::move(handler));
}

chrono::Timer::Handler Wallet_connection_impl::block_resubmit_handler(std::uint16_t timer_interval)
{
    return[self = shared_from_this(), timer_interval]()
    {
        self->m_logger->warn("Re-Submitting Block...");
        self->m_connection->transmit(self->m_submit_block_packet.get_bytes());

        // start timer again
        self->m_block_resubmit_timer->start(chrono::Seconds(2), self->block_resubmit_handler(2U));
    };

}

}