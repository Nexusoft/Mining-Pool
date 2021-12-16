#include "pool/session_impl.hpp"
#include "pool/miner_connection.hpp"
#include "LLC/random.h"
#include "TAO/Register/types/address.h"
#include "common/types.hpp"
#include <assert.h>

namespace nexuspool
{

Session_impl::Session_impl(persistance::Shared_data_writer::Sptr data_writer, Shared_data_reader::Sptr data_reader, common::Mining_mode mining_mode)
	: m_data_writer{ std::move(data_writer) }
	, m_data_reader{ std::move(data_reader) }
	, m_user_data{}
	, m_miner_connection{}
	, m_update_time{ std::chrono::steady_clock::now() }
	, m_hashrate_helper{ mining_mode }
	, m_block{}
{
}

void Session_impl::update_connection(std::shared_ptr<Miner_connection> miner_connection)
{
	m_miner_connection = std::move(miner_connection);
}

bool Session_impl::add_share()
{
	m_user_data.m_account = m_data_reader->get_account(m_user_data.m_account.m_address);
	m_user_data.m_account.m_shares++;
	m_hashrate_helper.add_share();
	return m_data_writer->update_account(m_user_data.m_account);
}

void Session_impl::reset_shares()
{
	m_user_data.m_account = m_data_reader->get_account(m_user_data.m_account.m_address);
	m_user_data.m_account.m_shares = 0;
	m_data_writer->update_account(m_user_data.m_account);
}

void Session_impl::update_hashrate(std::uint32_t pool_nbits, std::uint32_t network_nbits, double prime_shares_to_blocks_ratio)
{
	m_user_data.m_account = m_data_reader->get_account(m_user_data.m_account.m_address);
	m_user_data.m_account.m_hashrate = m_hashrate_helper.get_hashrate(pool_nbits, network_nbits, prime_shares_to_blocks_ratio);
	m_data_writer->update_account(m_user_data.m_account);
}

bool Session_impl::create_account()
{
	assert(m_user_data.m_new_account);
	assert(!m_user_data.m_account.m_address.empty());

	return m_data_writer->create_account(m_user_data.m_account.m_address);
}

std::unique_ptr<LLP::CBlock> Session_impl::get_block()
{
	if (m_block)
	{
		return std::move(m_block);
	}
	return nullptr;
}

// ------------------------------------------------------------------------------------------------------------

Session_registry_impl::Session_registry_impl(persistance::Data_reader::Uptr data_reader,
	persistance::Shared_data_writer::Sptr data_writer,
	nexus_http_interface::Component::Sptr http_interface,
	std::uint32_t session_expiry_time,
	common::Mining_mode mining_mode)
	: m_data_reader{ std::make_shared<Shared_data_reader>(std::move(data_reader)) }
	, m_data_writer{ std::move(data_writer) }
	, m_http_interface{std::move(http_interface)}
	, m_sessions{}
	, m_session_expiry_time{ session_expiry_time }
	, m_mining_mode{mining_mode}
{}

void Session_registry_impl::stop()
{
	std::scoped_lock lock(m_sessions_mutex);

	m_sessions.clear();
}

Session_key Session_registry_impl::create_session()
{
	std::scoped_lock lock(m_sessions_mutex);

	auto const session_key = LLC::GetRand256();
	m_sessions.emplace(std::make_pair(session_key, std::make_shared<Session_impl>(m_data_writer, m_data_reader, m_mining_mode)));
	return session_key;
}

std::shared_ptr<Session> Session_registry_impl::get_session(Session_key key)
{
	std::scoped_lock lock(m_sessions_mutex);
	if (m_sessions.count(key) == 0)
	{
		return nullptr;
	}
	return m_sessions[key];

}

void Session_registry_impl::clear_unused_sessions()
{
	std::scoped_lock lock(m_sessions_mutex);
	auto time_now = std::chrono::steady_clock::now();
	auto iter = m_sessions.begin();
	while (iter != m_sessions.end())
	{
		// delete sessions were the session is expired
		if (std::chrono::duration_cast<std::chrono::seconds>(time_now - iter->second->get_update_time()).count() > m_session_expiry_time)
		{
			iter = m_sessions.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Session_registry_impl::end_round()
{
	std::scoped_lock lock(m_sessions_mutex);

	for (auto& session : m_sessions)
	{
		session.second->reset_shares();
	}
}

std::size_t Session_registry_impl::get_sessions_size()
{
	std::scoped_lock lock(m_sessions_mutex);
	return m_sessions.size();
}

void Session_registry_impl::update_height(std::uint32_t height)
{
	std::scoped_lock lock(m_sessions_mutex);

	for (auto& session : m_sessions)
	{
		auto miner_connection = session.second->get_connection();
		auto miner_connection_shared = miner_connection.lock();
		if (miner_connection_shared)
		{
			miner_connection_shared->set_current_height(height);
		}
	}
}

bool Session_registry_impl::valid_nxs_address(std::string const& nxs_address)
{
	// check if nxs_address has a valid format
	TAO::Register::Address address_check{ nxs_address };
	if (!address_check.IsValid())
	{
		return false;
	}

	// check if nxs_address is registered on blockchain
	return m_http_interface->does_account_exists(nxs_address);
}

bool Session_registry_impl::does_account_exists(std::string account)
{
	return m_data_reader->does_account_exists(std::move(account));
}

void Session_registry_impl::login(Session_key key)
{
	auto session = get_session(key);
	auto& user_data = session->get_user_data();
	persistance::Account_data account_data{};
	{
		account_data = m_data_reader->get_account(user_data.m_account.m_address);
	}

	if (!account_data.is_empty())
	{
		// only move account_data from storage if the account has been found
		user_data.m_account = std::move(account_data);
	}

	user_data.m_login_time = std::chrono::steady_clock::now();
}

}