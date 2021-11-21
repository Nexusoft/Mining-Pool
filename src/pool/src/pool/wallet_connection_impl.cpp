#include "pool/wallet_connection_impl.hpp"
#include "pool/pool_manager.hpp"
#include "packet.hpp"
#include "block.hpp"
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
{
}

void Wallet_connection_impl::stop()
{
    m_timer_manager.stop();

    // close connection
    m_connection.reset();
}

void Wallet_connection_impl::retry_connect(network::Endpoint const& wallet_endpoint)
{
    m_connection = nullptr;		// close connection (socket etc)

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

                    Packet packet;
                    packet.m_header = Packet::SET_CHANNEL;
                    packet.m_length = 4;
                    packet.m_data = std::make_shared<network::Payload>(uint2bytes(self->m_mining_mode == common::Mining_mode::PRIME ? 1U : 2U));
                    self->m_connection->transmit(packet.get_bytes());

                    self->m_timer_manager.start_get_height_timer(self->m_get_height_interval, self->m_connection);
                }
                else
                {	// data received
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

    Packet packet{ std::move(receive_buffer) };
    if (!packet.is_valid())
    {
        // log invalid packet
        m_logger->error("Received packet is invalid. Header: {}", packet.m_header);
        return;
    }

    if (packet.m_header == Packet::PING)
    {
        m_logger->trace("Ping received");
    }
    else if (packet.m_header == Packet::BLOCK_HEIGHT)
    {
        auto pool_manager_shared = m_pool_manager.lock();
        if (!pool_manager_shared)
            return;

        auto const height = bytes2uint(*packet.m_data);
        if (height > m_current_height)
        {
            m_current_height = height;
            m_logger->info("Nexus Network: New Block with height {}", m_current_height);

            // update height at pool_manager
            pool_manager_shared->set_current_height(m_current_height);

            // get new block from wallet for pool_manager
            Packet packet_get_block;
            packet_get_block.m_header = Packet::GET_BLOCK;
            m_connection->transmit(packet_get_block.get_bytes());
            m_get_block_pool_manager = true; 

            // clear pending get_block handlers
            std::scoped_lock lock(m_get_block_mutex);
            std::queue<Get_block_data> empty_queue;
            std::swap(m_pending_get_blocks, empty_queue);
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
                    return;

                pool_manager_shared->set_block(std::move(block));
                m_get_block_pool_manager = false;
            }
            else
            {
                // get oldest pending_get_block handler from miner_connection and call it then pop()
                std::scoped_lock lock(m_get_block_mutex);
                if (!m_pending_get_blocks.empty())
                {
                    auto& get_block_data = m_pending_get_blocks.front();
                    get_block_data.m_timer->cancel();
                    get_block_data.m_handler(block);
                    m_pending_get_blocks.pop();
                }
            }
        }
        else
        {
            m_logger->warn("Block Obsolete Height = {}, Skipping over.", block.nHeight);
        }
    }
    else if (packet.m_header == Packet::ACCEPT)
    {
        m_logger->info("Block Accepted By Nexus Network.");
        auto pool_manager_shared = m_pool_manager.lock();
        if (!pool_manager_shared)
            return;

        // get_height immediately to get the next block faster than waiting on get_height_timer
        Packet packet_get_height;
        packet_get_height.m_header = Packet::GET_HEIGHT;
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
        m_logger->warn("Block Rejected by Nexus Network.");


        Packet packet_get_block;
        packet_get_block.m_header = Packet::GET_BLOCK;
        m_connection->transmit(packet_get_block.get_bytes());
        //  m_stats_collector->block_rejected();

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

void Wallet_connection_impl::submit_block(network::Shared_payload&& block_data, std::uint32_t block_map_id, Submit_block_handler&& handler)
{
    m_logger->info("Submitting Block...");

    Packet packet;
    packet.m_header = Packet::SUBMIT_BLOCK;
    packet.m_data = std::move(block_data);
    packet.m_length = 72;

    m_connection->transmit(packet.get_bytes());

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

    Packet packet_get_block;
    packet_get_block.m_header = Packet::GET_BLOCK;
    m_connection->transmit(packet_get_block.get_bytes());

    // store block request handler in pending list (handler comes from miner_connection)
    std::scoped_lock lock(m_get_block_mutex);
    Get_block_data get_block_data{ handler, m_timer_factory->create_timer() };
    std::weak_ptr<Wallet_connection_impl> weak_self = shared_from_this();
    get_block_data.m_timer->start(chrono::Seconds(3), [weak_self](auto canceled)
        {
            auto self = weak_self.lock();
            if (self && !canceled)
            {
                Packet packet_get_block;
                packet_get_block.m_header = Packet::GET_BLOCK;
                self->m_connection->transmit(packet_get_block.get_bytes());
            }

        });
    m_pending_get_blocks.emplace(std::move(get_block_data));
}

}