#include "pool/miner_connection_impl.hpp"
#include "pool/pool_manager.hpp"
#include "packet.hpp"
#include "block.hpp"
#include "TAO/Register/types/address.h"
#include "pool/types.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool
{
Miner_connection_impl::Miner_connection_impl(std::shared_ptr<spdlog::logger> logger,
	network::Connection::Sptr&& connection, 
	std::weak_ptr<Pool_manager> pool_manager,
	Session_key session_key,
	Session_registry::Sptr session_registry)
    : m_logger{ std::move(logger) }
	, m_connection{ std::move(connection) }
	, m_pool_manager{std::move(pool_manager)}
    , m_current_height{ 0 }
	, m_remote_address{""}
	, m_connection_closed{true}
	, m_session_key{session_key}
	, m_session_registry{std::move(session_registry)}
{
}

void Miner_connection_impl::stop()
{
    m_connection->close();
}

network::Connection::Handler Miner_connection_impl::connection_handler()
{
	std::weak_ptr<Miner_connection_impl> weak_self = shared_from_this();
	return[weak_self](network::Result::Code result, network::Shared_payload&& receive_buffer)
	{
		auto self = weak_self.lock();
		if (!self)
		{
			return;
		}

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

void Miner_connection_impl::process_data(network::Shared_payload&& receive_buffer)
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
        m_logger->error("Received packet is invalid. Header: {0}", packet.m_header);
        return;
    }

	auto session = m_session_registry->get_session(m_session_key);
	if (!session)
	{
		m_logger->trace("process_data, session invalid");
		return;
	}

    if (packet.m_header == Packet::PING)
    {
        Packet response;
        response = response.get_packet(Packet::PING);
        m_connection->transmit(response.get_bytes());
    }
	else if (packet.m_header == Packet::LOGIN)
	{
		auto& user_data = session->get_user_data();
		// check if already logged in
		if (user_data.m_logged_in)
		{
			m_logger->warn("Multiple login attempts of user {} with ip {} ", user_data.m_account.m_address, m_remote_address);
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
		user_data.m_new_account = !m_session_registry->does_account_exists(nxs_address);
		// login the user (fetch data from storage)
		user_data.m_account.m_address = nxs_address;
		m_session_registry->login(m_session_key);
		user_data.m_logged_in = true;

		response = response.get_packet(Packet::LOGIN_SUCCESS);
		m_connection->transmit(response.get_bytes());
	}
	else if (packet.m_header == Packet::GET_BLOCK)
	{
		auto pool_manager_shared = m_pool_manager.lock();
		if (pool_manager_shared)
		{
			m_pool_nbits = pool_manager_shared->get_pool_nbits();
			std::weak_ptr<Miner_connection_impl> weak_self = shared_from_this();
			pool_manager_shared->get_block([weak_self](auto block)
			{
				auto self = weak_self.lock();
				if (!self)
				{
					self->m_logger->debug("GET_BLOCK, miner_connection invalid.");
					return;
				}

				auto session = self->m_session_registry->get_session(self->m_session_key);
				if (!session)
				{
					self->m_logger->debug("GET_BLOCK, session invalid.");
					return;
				}
				session->set_block(block);

				//prepend pool nbits to the packet
				auto pool_nbits_bytes = nexuspool::uint2bytes(self->m_pool_nbits);
				Packet response;
				response.m_header = Packet::BLOCK_DATA;
				auto block_data = block.serialize();
				block_data.insert(block_data.begin(), pool_nbits_bytes.begin(), pool_nbits_bytes.end());
				response.m_length = block_data.size();
				response.m_data = std::make_shared<network::Payload>(block_data);

				self->m_connection->transmit(response.get_bytes());
			});
		}
	}
	//miner has submitted a block to the pool
	else if (packet.m_header == Packet::SUBMIT_BLOCK)
	{
		if (packet.m_length != 72)
		{
			m_logger->error("Invalid paket length for submit_block received! Received {} bytes", packet.m_length);
			return; // exit early
		}
		auto pool_manager_shared = m_pool_manager.lock();
		if (pool_manager_shared)
		{
			std::vector<uint8_t> block_data{ packet.m_data->begin(), packet.m_data->end() - 8 };			
			std::uint64_t nonce = bytes2uint64(std::vector<uint8_t>(packet.m_data->end() - 8, packet.m_data->end()));
			auto block = session->get_block();
			if (!block)
			{
				m_logger->debug("Miner has no block in current session set.");
				return; // exit early
			}

			block->nNonce = nonce;	// update nonce
			//TODO compare block merkle_root with received merkle_root (block_data)

			//update hashrate
			std::weak_ptr<Miner_connection_impl> weak_self = shared_from_this();
			pool_manager_shared->submit_block(std::move(block), session->get_user_data().m_account.m_address, [weak_self](auto result)
			{
				auto self = weak_self.lock();
				if (!self)
				{
					self->m_logger->debug("SUBMIT_BLOCK handler, miner_connection invalid.");
					return;
				}

				Packet response;
				if (result == Submit_block_result::accept)
				{
					self->process_accepted();
					response = response.get_packet(Packet::ACCEPT);
					self->m_connection->transmit(response.get_bytes());
				}
				else if(result == Submit_block_result::reject)
				{
					response = response.get_packet(Packet::REJECT);
					self->m_connection->transmit(response.get_bytes());
				}
				else
				{
					self->process_accepted();
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
	session->set_update_time(std::chrono::steady_clock::now());
}

void Miner_connection_impl::set_current_height(std::uint32_t height)
{
	m_current_height = height;
	if (!m_connection)
	{
		return;
	}

	Packet response;
	response.m_header = Packet::BLOCK_HEIGHT;
	response.m_data = std::make_shared<std::vector<uint8_t>>(uint2bytes(m_current_height));
	response.m_length = response.m_data->size();

	m_connection->transmit(response.get_bytes());
}

void Miner_connection_impl::process_accepted()
{
	auto session = m_session_registry->get_session(m_session_key);
	if (!session)
	{
		m_logger->trace("process_accepted, session invalid");
		return;
	}
	auto& user_data = session->get_user_data();

	// create new account
	if(user_data.m_new_account)
	{
		m_logger->info("Creating new account for miner {}", user_data.m_account.m_address);
		if (!session->create_account())
		{
			m_logger->error("Failed creating new account for miner {}", user_data.m_account.m_address);
		}
		user_data.m_new_account = false;
	}

	// add share
	if (!session->add_share(m_pool_nbits))
	{
		m_logger->error("Failed to update account for miner {}", user_data.m_account.m_address);
	}

	session->set_update_time(std::chrono::steady_clock::now());
}

}