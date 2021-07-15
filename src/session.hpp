#ifndef NEXUSPOOL_SESSION_HPP
#define NEXUSPOOL_SESSION_HPP

#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <chrono>
#include "LLC/types/uint1024.h"

namespace nexuspool
{
class Miner_connection;

struct Session_user
{
	std::string m_nxs_address{ "" };
	bool m_logged_in{ false };
};

using Session_key = uint256_t;

// Holds relevant user data and miner_connection
class Session
{
public:

	Session();

	void update_connection(std::shared_ptr<Miner_connection> miner_connection);
	std::weak_ptr<Miner_connection> get_connection() { return m_miner_connection; }
	Session_user& get_user_data() { return m_user_data;  }
	std::chrono::steady_clock::time_point get_update_time() const { return m_update_time; }
	void set_update_time(std::chrono::steady_clock::time_point update_time) { m_update_time = update_time; }

private:

	Session_user m_user_data;
	std::shared_ptr<Miner_connection> m_miner_connection;
	std::chrono::steady_clock::time_point m_update_time;


};

// Manages all sessions
class Session_registry
{
public:

	explicit Session_registry(std::uint32_t session_expiry_time);

	void stop();

	Session_key create_session();
	Session get_session(Session_key key);
	void update_session(Session_key key, Session& session);
	void clear_unused_sessions();

	// update height on active sessions
	void update_height(std::uint32_t height);

private:

	std::mutex m_sessions_mutex;
	std::map<Session_key, Session> m_sessions;
	std::uint32_t m_session_expiry_time;

};

}

#endif
