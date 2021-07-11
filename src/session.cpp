#include "session.hpp"
#include "miner_connection.hpp"

namespace nexuspool
{

Session::Session(std::uint64_t internal_id)
	: m_internal_id{ internal_id }
	, m_user_data{}
	, m_miner_connection{}
{
}

void Session::update_connection(std::shared_ptr<Miner_connection> miner_connection)
{
	m_miner_connection = std::move(miner_connection);
}

void Session::update_user_data(Session_user const& user_data)
{
	m_user_data = user_data;
}
// ------------------------------------------------------------------------------------------------------------

Session_registry::Session_registry()
	: m_internal_id_counter{0}
	, m_sessions{}
{}

Session& Session_registry::create_session()
{
	m_sessions.emplace_back(Session(m_internal_id_counter++));
	return m_sessions.back();
}



}