#ifndef NEXUSPOOL_POOL_SESSION_MOCK_HPP
#define NEXUSPOOL_POOL_SESSION_MOCK_HPP

#include <gmock/gmock.h>
#include "pool/session.hpp"

namespace nexuspool
{
class Session_mock : public Session
{
public:

	MOCK_METHOD(void, update_connection, (std::shared_ptr<Miner_connection> miner_connection), (override));
	MOCK_METHOD(std::weak_ptr<Miner_connection>, get_connection, (), (override));
	MOCK_METHOD(Session_user, get_user_data, (), (const override));
	MOCK_METHOD(void, update_user_data, (Session_user const& user_data), (override));
	MOCK_METHOD(std::chrono::steady_clock::time_point, get_update_time, (), (const override));
	MOCK_METHOD(void, set_update_time, (std::chrono::steady_clock::time_point update_time), (override));
	MOCK_METHOD(bool, add_share, (), (override));
	MOCK_METHOD(void, reset_shares, (), (override));
	MOCK_METHOD(void, update_hashrate, (double hashrate), (override));
	MOCK_METHOD(void, set_block, (LLP::CBlock const& block), (override));
	MOCK_METHOD(std::unique_ptr<LLP::CBlock>, get_block, (), (override));
	MOCK_METHOD(bool, create_account, (), (override));
	MOCK_METHOD(void, login, (), (override));
	MOCK_METHOD(bool, is_inactive, (), (const override));
	MOCK_METHOD(void, set_inactive, (), (override));
	MOCK_METHOD(bool, is_need_work, (), (const override));
	MOCK_METHOD(void, needs_work, (bool need_work), (override));
};

class Session_registry_mock : public Session_registry
{
public:

	MOCK_METHOD(void, stop, (), (override));
	MOCK_METHOD(Session_key, create_session, (), (override));
	MOCK_METHOD(std::shared_ptr<Session>, get_session, (Session_key key), (override));
	MOCK_METHOD(std::shared_ptr<Session>, get_session_with_no_work, (), (override));
	MOCK_METHOD(void, reset_work_status_of_sessions, (), (override));
	MOCK_METHOD(void, clear_unused_sessions, (), (override));
	MOCK_METHOD(void, end_round, (), (override));
	MOCK_METHOD(std::size_t, get_sessions_size, (), (override));
	MOCK_METHOD(bool, valid_nxs_address, (std::string const& nxs_address), (override));
	MOCK_METHOD(bool, does_account_exists, (std::string account), (override));
	MOCK_METHOD(void, get_hashrate, (), (override));
	MOCK_METHOD(void, send_notification, (std::string message), (override));
};


}

#endif
