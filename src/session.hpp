#ifndef NEXUSPOOL_SESSION_HPP
#define NEXUSPOOL_SESSION_HPP

#include <vector>
#include <memory>
#include <string>
#include <cstdint>

namespace nexuspool
{
class Miner_connection;

struct Session_user
{
	std::string m_nxs_address{ "" };
	bool m_logged_in{ false };
};

// Holds relevant user data and miner_connection
class Session
{
public:

	explicit Session(std::uint64_t internal_id);

	void update_connection(std::shared_ptr<Miner_connection> miner_connection);
	std::weak_ptr<Miner_connection> get_connection() { return m_miner_connection; }
	void update_user_data(Session_user const& user_data);
	Session_user& get_user_data() { return m_user_data;  }

private:

	std::uint64_t m_internal_id;
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

	Session& create_session();

private:

	std::uint64_t m_internal_id_counter;
	std::vector<Session> m_sessions;

};

}

#endif
