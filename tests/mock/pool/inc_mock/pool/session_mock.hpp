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
	MOCK_METHOD(Session_user&, get_user_data, (), (override));
	MOCK_METHOD(std::chrono::steady_clock::time_point, get_update_time, (), (const override));
	MOCK_METHOD(void set_update_time, (std::chrono::steady_clock::time_point update_time), (override));
	MOCK_METHOD(bool, add_share, (std::uint32_t pool_nbits), (override));
	MOCK_METHOD(double, get_hashrate, (), (const override));
	MOCK_METHOD(void, set_block, (LLP::CBlock const& block), (override));
	MOCK_METHOD(std::unique_ptr<LLP::CBlock>, get_block, (), (override));
	MOCK_METHOD(bool, create_account, (), (override));
};

class Session_registry_mock : public Session_registry
{
public:

	MOCK_METHOD(void, stop, (), (override));
	MOCK_METHOD(Session_key, create_session, (), (override));
	MOCK_METHOD(std::shared_ptr<Session>, get_session, (Session_key key), (override));
	MOCK_METHOD(void, clear_unused_sessions, (), (override));
	MOCK_METHOD(void, update_height, (std::uint32_t height), (override));
	MOCK_METHOD(bool, does_account_exists, (std::string account), (override));
	MOCK_METHOD(void, login, (Session_key key), (override));
};

}

#endif
