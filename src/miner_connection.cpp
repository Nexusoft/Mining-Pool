#include "miner_connection.hpp"
#include "pool_manager.hpp"
#include "packet.hpp"
#include "LLP/block.hpp"
#include "TAO/Register/types/address.h"

namespace nexuspool
{
Miner_connection::Miner_connection(chrono::Timer_factory::Sptr timer_factory, 
	network::Connection::Sptr&& connection, 
	std::weak_ptr<Pool_manager> pool_manager,
	Session_key session_key,
	Session_registry& session_registry)
    : m_connection{ std::move(connection) }
	, m_pool_manager{std::move(pool_manager)}
    , m_logger{ spdlog::get("logger") }
    , m_timer_manager{ std::move(timer_factory) }
    , m_current_height{ 0 }
	, m_remote_address{""}
	, m_connection_closed{true}
	, m_session_key{session_key}
	, m_session_registry{session_registry}
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

	auto session = m_session_registry.get_session(m_session_key);

    if (packet.m_header == Packet::PING)
    {
        Packet response;
        response = response.get_packet(Packet::PING);
        m_connection->transmit(response.get_bytes());
    }
	else if (packet.m_header == Packet::LOGIN)
	{
		auto user_data = session.get_user_data();
		// check if already logged in
		if (user_data.m_logged_in)
		{
			m_logger->warn("Multiple login attempts of user {} with ip {} ", user_data.m_nxs_address, m_remote_address);
			// increase ddos score
			return;
		}

		Packet response;

		Packet login_fail_response;
		login_fail_response = login_fail_response.get_packet(Packet::LOGIN_FAIL);
		
		auto const nxs_address = std::string(packet.m_data->begin(), packet.m_data->end());
		TAO::Register::Address address_check{ nxs_address };
		if (!address_check.IsValid())
		{
			m_logger->warn("Bad Account {}", nxs_address);
			//if (m_isDDOS)
			//	m_ddos->Ban(m_logger, "Invalid Nexus Address on Login");

			m_connection->transmit(login_fail_response.get_bytes());
			return;
		}

		// check if banned ip/user

		// check if user already exists in db
		// update db
		user_data.m_logged_in = true;
		response = response.get_packet(Packet::LOGIN_SUCCESS);
		m_connection->transmit(response.get_bytes());
	}
	else if (packet.m_header == Packet::GET_HEIGHT)
	{
		Packet response;
		response.m_header = Packet::BLOCK_HEIGHT;
		response.m_data = std::make_shared<std::vector<uint8_t>>(uint2bytes(m_current_height));
		response.m_length = packet.m_data->size();

		m_connection->transmit(response.get_bytes());
	}
	else if (packet.m_header == Packet::GET_BLOCK)
	{
		auto pool_manager_shared = m_pool_manager.lock();
		if (pool_manager_shared)
		{
			pool_manager_shared->get_block([self = shared_from_this()](auto block)
			{
				Packet response;
				response.m_header = Packet::BLOCK_DATA;
				auto block_data = block.Serialize();
				response.m_length = block_data.size();
				response.m_data = std::make_shared<network::Payload>(block_data);

				self->m_connection->transmit(response.get_bytes());
			});
		}
	}
	//miner has submitted a block to the pool
	else if (packet.m_header == Packet::SUBMIT_BLOCK)
	{
		auto pool_manager_shared = m_pool_manager.lock();
		if (pool_manager_shared)
		{
			
			std::vector<uint8_t> block_data{ packet.m_data->begin(), packet.m_data->end() - 8 };
			std::uint64_t nonce = bytes2uint64(std::vector<uint8_t>(packet.m_data->end() - 8, packet.m_data->end()));
			pool_manager_shared->submit_block(block_data, nonce, [self = shared_from_this()](auto result)
			{
				Packet response;
				if (result == Pool_manager::accept)
				{
					response = response.get_packet(Packet::ACCEPT);
					self->m_connection->transmit(response.get_bytes());
				}
				else if(result == Pool_manager::reject)
				{
					response = response.get_packet(Packet::REJECT);
					self->m_connection->transmit(response.get_bytes());
				}
				else
				{
					response = response.get_packet(Packet::BLOCK);
					self->m_connection->transmit(response.get_bytes());
				}
			});
		}
	}
    else
    {
        m_logger->error("Invalid header received.");
		return; // exit early
    }

	// received a valid paket from miner -> update session
	m_session_registry.update_session(m_session_key, session);
}

void Miner_connection::set_current_height(std::uint32_t height)
{
	m_current_height = height;
}

}