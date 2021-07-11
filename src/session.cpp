#include "session.hpp"
#include "miner_connection.hpp"
#include "LLC/random.h"

namespace nexuspool
{

Session::Session()
	: m_user_data{}
	, m_miner_connection{}
{
}

void Session::update_connection(std::shared_ptr<Miner_connection> miner_connection)
{
	m_miner_connection = std::move(miner_connection);
}

// ------------------------------------------------------------------------------------------------------------

Session_registry::Session_registry() : m_sessions{}
{}

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
void Session_registry::update_session(Session_key key, Session const& session)
{
	std::scoped_lock lock(m_sessions_mutex);
	m_sessions[key] = session;
}



}