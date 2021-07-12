#include "session.hpp"
#include "miner_connection.hpp"
#include "LLC/random.h"

namespace nexuspool
{

Session::Session()
	: m_user_data{}
	, m_miner_connection{}
	, m_update_time{std::chrono::steady_clock::now()}
{
}

void Session::update_connection(std::shared_ptr<Miner_connection> miner_connection)
{
	m_miner_connection = std::move(miner_connection);
}

// ------------------------------------------------------------------------------------------------------------

Session_registry::Session_registry(std::uint32_t session_expiry_time) 
	: m_sessions{}
	, m_session_expiry_time{session_expiry_time}
{}

void Session_registry::stop()
{
	std::scoped_lock lock(m_sessions_mutex);

	m_sessions.clear();
}

Session_key Session_registry::create_session()
{
	std::scoped_lock lock(m_sessions_mutex);

	auto const session_key = LLC::GetRand256();
	m_sessions.emplace(std::make_pair(session_key, Session{}));
	return session_key;
}

Session Session_registry::get_session(Session_key key)
{
	std::scoped_lock lock(m_sessions_mutex);
	return m_sessions[key];

}
void Session_registry::update_session(Session_key key, Session& session)
{
	std::scoped_lock lock(m_sessions_mutex);
	session.set_update_time(std::chrono::steady_clock::now());
	m_sessions[key] = session;
}

void Session_registry::clear_unused_sessions()
{
	std::scoped_lock lock(m_sessions_mutex);
	auto time_now = std::chrono::steady_clock::now();
	auto iter = m_sessions.begin();
	while (iter != m_sessions.end()) 
	{
		// delete sessions were the session is expired
		if(std::chrono::duration_cast<std::chrono::seconds>(time_now - iter->second.get_update_time()).count() > m_session_expiry_time)
		{
			iter = m_sessions.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}



}