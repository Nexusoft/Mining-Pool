#include "pool/timer_manager.hpp"
#include "network/endpoint.hpp"
#include "network/connection.hpp"
#include "packet.hpp"
#include "pool/wallet_connection.hpp"

namespace nexuspool
{
Timer_manager::Timer_manager(chrono::Timer_factory::Sptr timer_factory)
: m_timer_factory{std::move(timer_factory)}
{
    m_connection_retry_timer = m_timer_factory->create_timer();
    m_get_height_timer = m_timer_factory->create_timer();
}

void Timer_manager::start_connection_retry_timer(std::uint16_t timer_interval, std::weak_ptr<Wallet_connection> worker_manager, 
    network::Endpoint const& wallet_endpoint)
{
    m_connection_retry_timer->start(chrono::Seconds(timer_interval), 
        connection_retry_handler(std::move(worker_manager), wallet_endpoint));
}

void Timer_manager::start_get_height_timer(std::uint16_t timer_interval, std::weak_ptr<network::Connection> connection)
{
    m_get_height_timer->start(chrono::Seconds(timer_interval), get_height_handler(timer_interval, std::move(connection)));
}

void Timer_manager::stop()
{
    m_connection_retry_timer->cancel();
    m_get_height_timer->cancel();
}

chrono::Timer::Handler Timer_manager::connection_retry_handler(std::weak_ptr<Wallet_connection> worker_manager,
    network::Endpoint const& wallet_endpoint)
{
    return[worker_manager, wallet_endpoint](bool canceled)
    {
        if (canceled)	// don't do anything if the timer has been canceled
        {
            return;
        }

        auto worker_manager_shared = worker_manager.lock();
        if(worker_manager_shared)
        {
            worker_manager_shared->connect(wallet_endpoint);
        }
    }; 
}

chrono::Timer::Handler Timer_manager::get_height_handler(std::uint16_t get_height_interval, std::weak_ptr<network::Connection> connection)
{
    return[this, connection, get_height_interval](bool canceled)
    {
        if (canceled)	// don't do anything if the timer has been canceled
        {
            return;
        }

        auto connection_shared = connection.lock();
        if(connection_shared)
        {
            Packet packet_get_height;
            packet_get_height.m_header = Packet::GET_HEIGHT;
            connection_shared->transmit(packet_get_height.get_bytes());

            // restart timer
            m_get_height_timer->start(chrono::Seconds(get_height_interval), 
                get_height_handler(get_height_interval, std::move(connection_shared)));
        }
    }; 
}

}