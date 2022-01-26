#include "pool/miner_connection_legacy_impl.hpp"
#include "pool/pool_manager_impl.hpp"
#include "LLP/block.hpp"
#include "LLP/pool_protocol.hpp"
#include "pool/types.hpp"
#include <spdlog/spdlog.h>
#include <json/json.hpp>
#include <string>

namespace nexuspool
{

Miner_connection_legacy_impl::Miner_connection_legacy_impl(std::shared_ptr<spdlog::logger> logger,
	network::Connection::Sptr&& connection,
	std::weak_ptr<Pool_manager> pool_manager,
	Session_key session_key,
	Session_registry::Sptr session_registry,
	chrono::Timer::Uptr get_block_timer)
	: m_logger{ std::move(logger) }
	, m_connection{ std::move(connection) }
	, m_pool_manager{ std::move(pool_manager) }
	, m_session_key{ session_key }
	, m_session_registry{ std::move(session_registry) }
	, m_get_block_timer{std::move(get_block_timer)}
	, m_pool_nbits{ 0 }
	, m_current_height{ 0U }
{
}

void Miner_connection_legacy_impl::stop()
{
	m_get_block_timer->stop();
	m_connection->close();
}

network::Connection::Handler Miner_connection_legacy_impl::connection_handler()
{
	std::weak_ptr<Miner_connection_legacy_impl> weak_self = shared_from_this();
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
			self->m_logger->error("Connection to {} was not successful. Result: {}", self->m_connection->remote_endpoint().to_string(), network::Result::code_to_string(result));
			self->m_connection.reset();
		}
		else if (result == network::Result::connection_closed)
		{
			auto session = self->m_session_registry->get_session(self->m_session_key);
			if (session)
			{
				session->set_inactive();
			}
			self->m_connection.reset();
			self->m_get_block_timer->stop();
		}
		else if (result == network::Result::connection_ok)
		{
			self->m_logger->info("Connection to {} established", self->m_connection->remote_endpoint().to_string());
			self->m_get_block_timer->start(chrono::Seconds(4), self->get_block_handler(4));
		}
		else
		{	// data received
			self->process_data(std::move(receive_buffer));
		}
	};
}

void Miner_connection_legacy_impl::process_data(network::Shared_payload&& receive_buffer)
{
	// if we don't have a connection to the wallet we cant do anything useful.
	if (!m_connection)
	{
		return;
	}

	auto remaining_size = receive_buffer->size();
	auto session = m_session_registry->get_session(m_session_key);
	do
	{
		auto packet = extract_packet_from_buffer(receive_buffer, remaining_size, receive_buffer->size() - remaining_size);
		if (!packet.is_valid())
		{
			// log invalid packet
			m_logger->error("Miner_connection_legacy: Received packet is invalid. Header: {0}", packet.m_header);
			continue;
		}

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
			process_login(std::move(packet), session);
		}
		else if (packet.m_header == Packet::GET_BLOCK)
		{
			// ignore get_block messages for now
		}
		//miner has submitted a block to the pool
		else if (packet.m_header == Packet::SUBMIT_BLOCK)
		{
			auto pool_manager_shared = m_pool_manager.lock();
			if (pool_manager_shared)
			{
				if (packet.m_length != 72)
				{
					m_logger->error("Invalid paket length for submit_block received! Received {} bytes", packet.m_length);
					continue;
				}
				std::vector<uint8_t> block_data{ packet.m_data->begin(), packet.m_data->end() - 8 };
				auto nonce = bytes2uint64(std::vector<uint8_t>(packet.m_data->end() - 8, packet.m_data->end()));

				auto block = session->get_block();
				if (!block)
				{
					m_logger->debug("Miner has no block in current session set.");
					return; // exit early
				}

				block->nNonce = nonce;	// update nonce
				//TODO compare block merkle_root with received merkle_root (block_data)

				std::weak_ptr<Miner_connection_legacy_impl> weak_self = shared_from_this();
				pool_manager_shared->submit_block(std::move(block), m_session_key, [weak_self](auto result)
					{
						auto self = weak_self.lock();
						if (!self)
						{
							self->m_logger->debug("SUBMIT_BLOCK handler, miner_connection invalid.");
							return;
						}
						if (!self->m_connection)
						{
							self->m_logger->debug("SUBMIT_BLOCK handler, miner_connection connection invalid.");
							return;
						}

						Packet response;
						if (result == Submit_block_result::accept)
						{
							self->process_accepted();
							response = response.get_packet(Packet::ACCEPT);
							self->m_connection->transmit(response.get_bytes());
							// immediately get a net block for miner
							auto pool_manager_shared_2 = self->m_pool_manager.lock();
 							if (pool_manager_shared_2)
							{
								self->get_block(std::move(pool_manager_shared_2));
							}
						}
						else if (result == Submit_block_result::reject)
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
		else if (packet.m_header == Packet::SET_CHANNEL)
		{
			m_logger->error("Invalid header received. Possibly from miner in SOLO mode");
			m_connection->close();
			return;
		}
		else if (packet.m_header == Packet::GET_HEIGHT)
		{
			m_logger->error("Invalid header received. Possibly from miner in SOLO mode");
			m_connection->close();
			return;
		}
		else
		{
			m_logger->error("Invalid header received.");
			continue;
		}
	} while (remaining_size != 0);

	// received a valid paket from miner -> update session
	session->set_update_time(std::chrono::steady_clock::now());
}

void Miner_connection_legacy_impl::process_accepted()
{
	auto session = m_session_registry->get_session(m_session_key);
	if (!session)
	{
		m_logger->trace("process_accepted, session invalid");
		return;
	}
	auto user_data = session->get_user_data();

	// create new account
	if (user_data.m_new_account)
	{
		m_logger->info("Creating new account for miner {}", user_data.m_account.m_address);
		if (!session->create_account())
		{
			m_logger->error("Failed creating new account for miner {}", user_data.m_account.m_address);
		}
		else
		{
			user_data.m_new_account = false;
			session->update_user_data(user_data);
		}
	}

	// add share
	if (!session->add_share())
	{
		m_logger->error("Failed to update account for miner {}", user_data.m_account.m_address);
	}

	session->set_update_time(std::chrono::steady_clock::now());
}


void Miner_connection_legacy_impl::process_login(Packet login_packet, std::shared_ptr<Session> session)
{
	auto& user_data = session->get_user_data();
	// check if already logged in
	if (user_data.m_logged_in)
	{
		m_logger->warn("Multiple login attempts of user {} with endpoint {} ", user_data.m_account.m_address, m_connection->remote_endpoint().to_string());
		// increase ddos score
		return;
	}

	Packet response;
	Packet login_fail_response;
	login_fail_response = login_fail_response.get_packet(Packet::LOGIN_FAIL);

	auto const nxs_address = std::string(login_packet.m_data->begin(), login_packet.m_data->end());
	auto const nxs_address_valid = m_session_registry->valid_nxs_address(nxs_address);
	if (!nxs_address_valid)
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
	session->update_user_data(user_data);
	session->login();

	response = response.get_packet(Packet::LOGIN_SUCCESS);
	m_connection->transmit(response.get_bytes());
}

void Miner_connection_legacy_impl::get_block(std::shared_ptr<Pool_manager> pool_manager)
{
	m_pool_nbits = pool_manager->get_pool_nbits();
	std::weak_ptr<Miner_connection_legacy_impl> weak_self = shared_from_this();
	pool_manager->get_block([weak_self](auto block)
		{
			auto self = weak_self.lock();
			if (!self)
			{
				self->m_logger->debug("GET_BLOCK handler, miner_connection_legacy invalid.");
				return;
			}
			if (!self->m_connection)
			{
				self->m_logger->debug("GET_BLOCK handler, miner_connection_legacy connection invalid.");
				return;
			}

			auto session = self->m_session_registry->get_session(self->m_session_key);
			if (!session)
			{
				self->m_logger->debug("GET_BLOCK handler, session invalid.");
				return;
			}
			session->set_block(block);

			//prepend pool nbits to the packet
			auto pool_nbits_bytes = nexuspool::uint2bytes(self->m_pool_nbits);

			auto block_data = block.serialize();
			block_data.insert(block_data.begin(), pool_nbits_bytes.begin(), pool_nbits_bytes.end());
			Packet response{ Packet::BLOCK_DATA, std::make_shared<network::Payload>(block_data) };

			self->m_connection->transmit(response.get_bytes());
		});
}

chrono::Timer::Handler Miner_connection_legacy_impl::get_block_handler(std::uint16_t get_block_interval)
{
	return[self = shared_from_this(), get_block_interval]()
	{
		auto pool_manager_shared = self->m_pool_manager.lock();
		if (pool_manager_shared)
		{
			self->get_block(std::move(pool_manager_shared));
		}

		// restart timer
		self->m_get_block_timer->start(chrono::Seconds(get_block_interval), self->get_block_handler(get_block_interval));
	};
}


}