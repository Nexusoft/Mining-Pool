#ifndef NEXUSPOOL_SESSION_HPP
#define NEXUSPOOL_SESSION_HPP

#include <memory>
#include <string>
#include <chrono>
#include "LLC/types/uint1024.h"
#include "persistance/types.hpp"
#include "block.hpp"

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

	virtual ~Session() = default;

	virtual void update_connection(std::shared_ptr<Miner_connection> miner_connection) = 0;
	virtual std::weak_ptr<Miner_connection> get_connection() = 0;
	virtual Session_user& get_user_data() = 0;
	virtual std::chrono::steady_clock::time_point get_update_time() const = 0;
	virtual void set_update_time(std::chrono::steady_clock::time_point update_time) = 0;
	virtual bool add_share(std::uint32_t pool_nbits) = 0;
	virtual double get_hashrate() const = 0;
	virtual void set_block(LLP::CBlock const& block) = 0;
	virtual std::unique_ptr<LLP::CBlock> get_block() = 0;

	virtual bool create_account() = 0;
};

// Manages all sessions
class Session_registry
{
public:
	using Sptr = std::shared_ptr<Session_registry>;

	virtual ~Session_registry() = default;

	virtual void stop() = 0;

	// Managment methods
	virtual Session_key create_session() = 0;
	virtual std::shared_ptr<Session> get_session(Session_key key) = 0;
	virtual void clear_unused_sessions() = 0;

	// update height on active sessions
	virtual void update_height(std::uint32_t height) = 0;

	virtual bool valid_nxs_address(std::string const& nxs_address) = 0;
	virtual bool does_account_exists(std::string account) = 0;
	virtual void login(Session_key key) = 0;	// fetch user data from storage for the specific session
};

}

#endif
