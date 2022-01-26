#ifndef NEXUSPOOL_SESSION_IMPL_HPP
#define NEXUSPOOL_SESSION_IMPL_HPP

#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <chrono>
#include <atomic>
#include "LLC/types/uint1024.h"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include "nexus_http_interface/component.hpp"
#include "pool/utils.hpp"
#include "pool/session.hpp"
#include "pool/shared_data_reader.hpp"
#include "LLP/block.hpp"

namespace nexuspool
{
class Miner_connection;

// Holds relevant user data and miner_connection
class Session_impl : public Session
{
public:

	Session_impl(persistance::Shared_data_writer::Sptr data_writer, Shared_data_reader::Sptr data_reader, common::Mining_mode mining_mode);
	~Session_impl();

	void update_connection(std::shared_ptr<Miner_connection> miner_connection) override;
	std::weak_ptr<Miner_connection> get_connection() override { return m_miner_connection; }
	Session_user get_user_data() const override { return m_user_data; }
	void update_user_data(Session_user const& user_data) override { m_user_data = user_data; }
	std::chrono::steady_clock::time_point get_update_time() const override { return m_update_time; }
	void set_update_time(std::chrono::steady_clock::time_point update_time) override { m_update_time = update_time; }
	bool add_share() override;
	void reset_shares() override;
	void update_hashrate(double hashrate) override;
	void set_block(LLP::CBlock const& block) override { m_block = std::make_unique<LLP::CBlock>(block); }
	std::unique_ptr<LLP::CBlock> get_block() override;
	bool is_inactive() const override { return m_inactive; }
	void set_inactive() { m_inactive = true; }
	bool is_need_work() const override { return m_work_needed;  }
	void needs_work(bool need_work) override { m_work_needed = need_work; }

	bool create_account() override;
	void login() override;

private:

	persistance::Shared_data_writer::Sptr m_data_writer;
	Shared_data_reader::Sptr m_data_reader;
	Session_user m_user_data;
	std::shared_ptr<Miner_connection> m_miner_connection;
	std::chrono::steady_clock::time_point m_update_time;
	Hashrate_helper m_hashrate_helper;
	std::unique_ptr<LLP::CBlock> m_block;
	std::atomic_bool m_inactive;
	std::atomic_bool m_work_needed;
};

// Manages all sessions
class Session_registry_impl : public Session_registry
{
public:

	Session_registry_impl(persistance::Data_reader::Uptr data_reader,
		persistance::Shared_data_writer::Sptr data_writer,
		nexus_http_interface::Component::Sptr http_interface,
		std::uint32_t session_expiry_time,
		common::Mining_mode mining_mode);

	void stop() override;

	// Managment methods
	Session_key create_session() override;
	std::shared_ptr<Session> get_session(Session_key key) override;
	std::shared_ptr<Session> get_session_with_no_work() override;
	void reset_work_status_of_sessions() override;
	void clear_unused_sessions() override;
	void end_round() override;
	std::size_t get_sessions_size() override;

	// sends get_hashrate message to active sessions
	void get_hashrate() override;
	// sends pool notification message to active sessions
	void send_notification(std::string message) override;

	bool valid_nxs_address(std::string const& nxs_address) override;
	bool does_account_exists(std::string account) override;

private:

	Shared_data_reader::Sptr m_data_reader;			// hold ownership over data_reader/writer
	persistance::Shared_data_writer::Sptr m_data_writer;
	nexus_http_interface::Component::Sptr m_http_interface;
	std::mutex m_sessions_mutex;
	std::map<Session_key, std::shared_ptr<Session>> m_sessions;
	std::uint32_t m_session_expiry_time;
	common::Mining_mode m_mining_mode;

};

}

#endif
