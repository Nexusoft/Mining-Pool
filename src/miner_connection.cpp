#include "miner_connection.hpp"
#include "packet.hpp"
#include "LLP/block.hpp"

namespace nexuspool
{
Miner_connection::Miner_connection(chrono::Timer_factory::Sptr timer_factory, network::Connection::Sptr&& connection)
    : m_connection{ std::move(connection) }
    , m_logger{ spdlog::get("logger") }
    , m_timer_manager{ std::move(timer_factory) }
    , m_current_height{ 0 }
	, m_remote_address{""}
	, m_connection_closed{true}
{
}

void Miner_connection::stop()
{
    m_timer_manager.stop();

    m_connection->close();
}

network::Connection::Handler Miner_connection::connection_handler()
{
	return[self = shared_from_this()](network::Result::Code result, network::Shared_payload&& receive_buffer)
	{
		if (result == network::Result::connection_declined ||
			result == network::Result::connection_aborted ||
			result == network::Result::connection_error)
		{
			self->m_logger->error("Connection to {0} was not successful. Result: {1}", self->m_remote_address, result);
			self->m_connection.reset();
			self->m_connection_closed = true;
		}
		else if (result == network::Result::connection_closed)
		{
			self->m_connection.reset();
			self->m_connection_closed = true;
		}
		else if (result == network::Result::connection_ok)
		{
			self->m_connection->remote_endpoint().address(self->m_remote_address);
			self->m_logger->info("Connection to {} established", self->m_remote_address);
		}
		else
		{	// data received
			self->process_data(std::move(receive_buffer));
		}
	};
}

void Miner_connection::process_data(network::Shared_payload&& receive_buffer)
{
    Packet packet{ std::move(receive_buffer) };
    if (!packet.is_valid())
    {
        // log invalid packet
        m_logger->error("Received packet is invalid. Header: {0}", packet.m_header);
        return;
    }

    if (packet.m_header == Packet::PING)
    {
        Packet response;
        response = response.get_packet(Packet::PING);
        m_connection->transmit(response.get_bytes());
    }
    else
    {
        m_logger->error("Invalid header received.");
    }
}
}