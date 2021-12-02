#ifndef NEXUSPOOL_SESSION_IMPL_HPP
#define NEXUSPOOL_SESSION_IMPL_HPP

#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <chrono>
#include "LLC/types/uint1024.h"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include "nexus_http_interface/component.hpp"
#include "pool/utils.hpp"
#include "pool/session.hpp"
#include "LLP/block.hpp"

namespace nexuspool
{
class Miner_connection;

// Holds relevant user data and miner_connection
class Session_impl : public Session
{
public:

	Session_impl(persistance::Shared_data_writer::Sptr data_writer);

	void update_connection(std::shared_ptr<Miner_connection> miner_connection) override;
	std::weak_ptr<Miner_connection> get_connection() override { return m_miner_connection; }
	Session_user& get_user_data() override { return m_user_data; }
	std::chrono::steady_clock::time_point get_update_time() const override { return m_update_time; }
	void set_update_time(std::chrono::steady_clock::time_point update_time) override { m_update_time = update_time; }
	bool add_share() override;
	double get_hashrate(std::uint32_t pool_nbits, std::uint32_t network_nbits, double prime_shares_to_blocks_ratio) override;
	void set_block(LLP::CBlock const& block) override { m_block = std::make_unique<LLP::CBlock>(block); }
	std::unique_ptr<LLP::CBlock> get_block() override;

	bool create_account() override;

private:

	persistance::Shared_data_writer::Sptr m_data_writer;
	Session_user m_user_data;
	std::shared_ptr<Miner_connection> m_miner_connection;
	std::chrono::steady_clock::time_point m_update_time;
	Hashrate_helper m_hashrate_helper;
	std::unique_ptr<LLP::CBlock> m_block;
};

// Manages all sessions
class Session_registry_impl : public Session_registry
{
public:

	Session_registry_impl(persistance::Data_reader::Uptr data_reader,
		persistance::Shared_data_writer::Sptr data_writer,
		nexus_http_interface::Component::Sptr http_interface,
		std::uint32_t session_expiry_time);

	void stop() override;

	// Managment methods
	Session_key create_session() override;
	std::shared_ptr<Session> get_session(Session_key key) override;
	void clear_unused_sessions() override;

	// update height on active sessions
	void update_height(std::uint32_t height) override;

	bool valid_nxs_address(std::string const& nxs_address) override;
	bool does_account_exists(std::string account) override;
	void login(Session_key key) override;	// fetch user data from storage for the specific session

private:

	persistance::Data_reader::Uptr m_data_reader;			// hold ownership over data_reader/writer
	persistance::Shared_data_writer::Sptr m_data_writer;
	nexus_http_interface::Component::Sptr m_http_interface;
	std::mutex m_sessions_mutex;
	std::mutex m_data_reader_mutex;
	std::map<Session_key, std::shared_ptr<Session>> m_sessions;
	std::uint32_t m_session_expiry_time;

};

}

#endif