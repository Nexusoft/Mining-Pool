#include "pool/miner_connection_impl.hpp"
#include "pool/pool_manager_impl.hpp"
#include "LLP/block.hpp"
#include "LLP/pool_protocol.hpp"
#include "pool/types.hpp"
#include <spdlog/spdlog.h>
#include <json/json.hpp>
#include <string>

namespace nexuspool
{
Miner_connection::Sptr create_miner_connection(std::shared_ptr<spdlog::logger> logger,
	network::Connection::Sptr connection,
	std::weak_ptr<Pool_manager> pool_manager,
	Session_key session_key,
	Session_registry::Sptr session_registry)
{
	return std::make_shared<Miner_connection_impl>(
		std::move(logger),
		std::move(connection),
		std::move(pool_manager),
		session_key,
		std::move(session_registry));
}

Miner_connection_impl::Miner_connection_impl(std::shared_ptr<spdlog::logger> logger,
	network::Connection::Sptr&& connection, 
	std::weak_ptr<Pool_manager> pool_manager,
	Session_key session_key,
	Session_registry::Sptr session_registry)
    : m_logger{ std::move(logger) }
	, m_connection{ std::move(connection) }
	, m_pool_manager{std::move(pool_manager)}
	, m_session_key{session_key}
	, m_session_registry{std::move(session_registry)}
	, m_pool_nbits{ 0 }
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
		}
		else if (result == network::Result::connection_ok)
		{
			self->m_logger->info("Connection to {} established", self->m_connection->remote_endpoint().to_string());
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

	auto remaining_size = receive_buffer->size();
	auto session = m_session_registry->get_session(m_session_key);
	do
	{
		auto packet = extract_packet_from_buffer(receive_buffer, remaining_size, receive_buffer->size() - remaining_size);
		if (!packet.is_valid())
		{
			// log invalid packet
			m_logger->error("Received packet is invalid. Header: {0}", packet.m_header);
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
		//miner has submitted a block to the pool
		else if (packet.m_header == Packet::SUBMIT_BLOCK)
		{
			// miner needs new work
			session->needs_work(true);

			if (packet.m_length != 72)
			{
				m_logger->error("Invalid paket length for submit_block received! Received {} bytes", packet.m_length);
				continue;
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

				std::weak_ptr<Miner_connection_impl> weak_self = shared_from_this();
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
		else if (packet.m_header == Packet::HASHRATE)
		{
			auto user_data = session->get_user_data();
			// only update hashrate if user is logged in and the account has already been created
			if (user_data.m_logged_in && !user_data.m_new_account)
			{
				auto const hashrate = bytes2double(*packet.m_data);
				session->update_hashrate(hashrate);
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
	}
	while (remaining_size != 0);

	// received a valid paket from miner -> update session
	session->set_update_time(std::chrono::steady_clock::now());
}

void Miner_connection_impl::process_accepted()
{
	auto session = m_session_registry->get_session(m_session_key);
	if (!session)
	{
		m_logger->trace("process_accepted, session invalid");
		return;
	}
	auto user_data = session->get_user_data();

	// create new account
	if(user_data.m_new_account)
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

void Miner_connection_impl::send_work(LLP::CBlock const& block)
{
	if (!m_connection)
	{
		return;
	}

	auto session = m_session_registry->get_session(m_session_key);
	if (!session)
	{
		m_logger->debug("SEND_WORK, session invalid.");
		return;
	}
	auto pool_manager_shared = m_pool_manager.lock();
	if (!pool_manager_shared)
	{
		return;
	}

	session->set_block(block);

	//prepend pool nbits to the packet
	m_pool_nbits = pool_manager_shared->get_pool_nbits();
	auto pool_nbits_bytes = nexuspool::uint2bytes(m_pool_nbits);
	auto block_data = block.serialize();
	block_data.insert(block_data.begin(), pool_nbits_bytes.begin(), pool_nbits_bytes.end());

	nlohmann::json j;
	j["work_id"] = 1;
	j["block"] = nlohmann::json::binary( block_data );
	auto j_string = j.dump();

	network::Payload work_data{ j_string.begin(), j_string.end() };
	Packet response{ Packet::WORK, std::make_shared<network::Payload>(work_data) };

	m_connection->transmit(response.get_bytes());
}

void Miner_connection_impl::get_hashrate()
{
	if (!m_connection)
	{
		return;
	}

	Packet packet{ Packet::GET_HASHRATE, nullptr };
	m_connection->transmit(packet.get_bytes());
}

void Miner_connection_impl::process_login(Packet login_packet, std::shared_ptr<Session> session)
{
	auto user_data = session->get_user_data();
	// check if already logged in
	if (user_data.m_logged_in)
	{
		m_logger->warn("Multiple login attempts of user {} with endpoint {} ", user_data.m_account.m_address, m_connection->remote_endpoint().to_string());
		// increase ddos score
		return;
	}

	nlohmann::json login_response_json;
	login_response_json["result_code"] = Pool_protocol_result::Success;
	login_response_json["result_message"] = "";

	std::string nxs_address, display_name;
	std::uint8_t protocol_version{ 0U };
	try
	{
		nlohmann::json j = nlohmann::json::parse(login_packet.m_data->begin(), login_packet.m_data->end());
		nxs_address = j.at("username");
		display_name = j.at("display_name");
		protocol_version = j.at("protocol_version");	// TODO: protocol version check in future
	}
	catch (std::exception& e)
	{
		m_logger->debug("Invalid login json. Exception: {}", e.what());
		return;
	}

	// protocol version check
	if (protocol_version < POOL_PROTOCOL_VERSION)
	{
		login_response_json["result_code"] = Pool_protocol_result::Protocol_version_fail;
		login_response_json["result_message"] = "Please update miner. Mandatory protocol_version " + std::to_string(POOL_PROTOCOL_VERSION);
		send_login_fail(login_response_json.dump());
		return;
	}

	auto const nxs_address_valid = m_session_registry->valid_nxs_address(nxs_address);
	if (!nxs_address_valid)
	{
		m_logger->warn("Bad Account {}", nxs_address);
		//if (m_isDDOS)
		//	m_ddos->Ban(m_logger, "Invalid Nexus Address on Login");

		login_response_json["result_code"] = Pool_protocol_result::Login_fail_invallid_nxs_account;
		login_response_json["result_message"] = "Invalid nxs account";
		send_login_fail(login_response_json.dump());
		return;
	}

	// check if banned ip/user

	// check if user already exists in db
	user_data.m_new_account = !m_session_registry->does_account_exists(nxs_address);
	// login the user (fetch data from storage)
	user_data.m_account.m_address = nxs_address;
	check_and_update_display_name(display_name, login_response_json);
	user_data.m_account.m_display_name = display_name;
	session->update_user_data(user_data);
	session->login();

	auto login_response_json_string = login_response_json.dump();

	network::Payload login_data{ login_response_json_string.begin(), login_response_json_string.end() };
	Packet response{ Packet::LOGIN_SUCCESS, std::make_shared<network::Payload>(login_data) };
	m_connection->transmit(response.get_bytes());
}

void Miner_connection_impl::send_login_fail(std::string json_string)
{
	network::Payload login_data{ json_string.begin(), json_string.end() };
	Packet login_fail_response{ Packet::LOGIN_FAIL, std::make_shared<network::Payload>(login_data) };
	m_connection->transmit(login_fail_response.get_bytes());
}

void Miner_connection_impl::check_and_update_display_name(std::string display_name, nlohmann::json& login_response)
{
	if (display_name.empty())
	{
		login_response["result_code"] = Pool_protocol_result::Login_warn_no_display_name;
		login_response["result_message"] = "No display_name set";
		return;
	}
}

void Miner_connection_impl::send_pool_notification(std::string message)
{
	nlohmann::json j;
	j["message"] = message;
	auto j_string = j.dump();

	Packet response{ Packet::POOL_NOTIFICATION, std::make_shared<network::Payload>(network::Payload{ j_string.begin(), j_string.end() }) };
//	m_connection->transmit(response.get_bytes());		TODO: enable when miner 1.5 released
}

}