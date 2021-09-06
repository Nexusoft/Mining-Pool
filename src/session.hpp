#ifndef NEXUSPOOL_SESSION_HPP
#define NEXUSPOOL_SESSION_HPP

#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <chrono>
#include "LLC/types/uint1024.h"
#include "persistance/data_writer.hpp"
#include "persistance/data_reader.hpp"
#include "persistance/types.hpp"
#include "utils.hpp"

namespace nexuspool
{
class Miner_connection;

struct Session_user
{
	bool m_logged_in{ false };
	bool m_new_account{ true };
	persistance::Account_data m_account;
	std::chrono::steady_clock::time_point m_login_time;
};

using Session_key = uint256_t;

// Holds relevant user data and miner_connection
class Session
{
public:

	Session(persistance::Shared_data_writer::Sptr data_writer);

	void update_connection(std::shared_ptr<Miner_connection> miner_connection);
	std::weak_ptr<Miner_connection> get_connection() { return m_miner_connection; }
	Session_user& get_user_data() { return m_user_data;  }
	std::chrono::steady_clock::time_point get_update_time() const { return m_update_time; }
	void set_update_time(std::chrono::steady_clock::time_point update_time) { m_update_time = update_time; }
	bool add_share(std::uint32_t pool_nbits);
	double get_hashrate() const;

	bool create_account();

private:

	persistance::Shared_data_writer::Sptr m_data_writer;
	Session_user m_user_data;
	std::shared_ptr<Miner_connection> m_miner_connection;
	std::chrono::steady_clock::time_point m_update_time;
	std::uint32_t m_shares_in_session{ 0U };	// submitted shares since login
	Hashrate_helper m_hashrate_helper;
};

// Manages all sessions
class Session_registry
{
public:

	explicit Session_registry(persistance::Data_reader::Uptr data_reader, 
		persistance::Shared_data_writer::Sptr data_writer,
		std::uint32_t session_expiry_time);

	void stop();

	// Managment methods
	Session_key create_session();
	std::shared_ptr<Session> get_session(Session_key key);
	void clear_unused_sessions();

	// update height on active sessions
	void update_height(std::uint32_t height);

	bool does_account_exists(std::string account);
	void login(Session_key key);	// fetch user data from storage for the specific session

private:

	persistance::Data_reader::Uptr m_data_reader;			// hold ownership over data_reader/writer
	persistance::Shared_data_writer::Sptr m_data_writer;
	std::mutex m_sessions_mutex;
	std::mutex m_data_reader_mutex;
	std::map<Session_key, std::shared_ptr<Session>> m_sessions;
	std::uint32_t m_session_expiry_time;

};

}

#endif
