#ifndef NEXUSPOOL_SESSION_HPP
#define NEXUSPOOL_SESSION_HPP

#include <map>
#include <memory>
#include <string>
#include <mutex>
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

private:

	Session_user m_user_data;
	std::shared_ptr<Miner_connection> m_miner_connection;
	// creation_time
	// update_time


};

// Manages all sessions
class Session_registry
{
public:

	Session_registry();

	Session_key create_session();
	Session get_session(Session_key key);
	void update_session(Session_key key, Session const& session);

private:

	std::mutex m_sessions_mutex;
	std::map<Session_key, Session> m_sessions;

};

}

#endif
