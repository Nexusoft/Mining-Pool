#include "persistance/sqlite/command/command_impl.hpp"
#include <array>

namespace nexuspool
{
namespace persistance
{
namespace command
{

void Command_base_database_sqlite::bind_param(sqlite3_stmt* stmt, const char* name, std::string const& value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		return;
	}

	sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

void Command_base_database_sqlite::bind_param(sqlite3_stmt* stmt, const char* name, int value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		return;
	}

	sqlite3_bind_int(stmt, index, value);
}

void Command_base_database_sqlite::bind_param(sqlite3_stmt* stmt, const char* name, int64_t value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		return;
	}

	sqlite3_bind_int64(stmt, index, value);
}

void Command_base_database_sqlite::bind_param(sqlite3_stmt* stmt, const char* name, double value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		return;
	}

	sqlite3_bind_double(stmt, index, value);
}

void Command_base_database_sqlite::reset()
{
	sqlite3_reset(m_stmt);
	sqlite3_clear_bindings(m_stmt);
}

// -----------------------------------------------------------------------------------------------

Command_banned_user_and_ip_impl::Command_banned_user_and_ip_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle,
		"SELECT user, ip FROM banned_users_connections WHERE user = :user AND ip = :ip;",
		-1, &m_stmt, NULL);
}

std::any Command_banned_user_and_ip_impl::get_command() const
{
	Command_type_sqlite command{ m_stmt, {{Column_sqlite::string}, {Column_sqlite::string }} };
	return command;
}

void Command_banned_user_and_ip_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::array<std::string, 2>>(m_params);
	bind_param(m_stmt, ":user", casted_params[0]);
	bind_param(m_stmt, ":ip", casted_params[1]);
}

// -----------------------------------------------------------------------------------------------

Command_banned_api_ip_impl::Command_banned_api_ip_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT ip FROM banned_connections_api WHERE ip = :ip;", -1, &m_stmt, NULL);
}

std::any Command_banned_api_ip_impl::get_command() const
{
	Command_type_sqlite command{ m_stmt, {{Column_sqlite::string}} };
	return command;
}

void Command_banned_api_ip_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":ip", casted_params);
}

// -----------------------------------------------------------------------------------------------

Command_account_exists_impl::Command_account_exists_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT COUNT(name) FROM account WHERE name = :name;", -1, &m_stmt, NULL);
}

std::any Command_account_exists_impl::get_command() const
{
	Command_type_sqlite command{ m_stmt, {{Column_sqlite::int32}} };
	return command;
}

void Command_account_exists_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":name", casted_params);
}

// -----------------------------------------------------------------------------------------------
Command_get_account_impl::Command_get_account_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT name, created_at, last_active, connection_count, shares, reward, hashrate FROM account WHERE name = :name;", -1, &m_stmt, NULL);
}

std::any Command_get_account_impl::get_command() const
{
	Command_type_sqlite command{ m_stmt, 
		{{Column_sqlite::string},
		{Column_sqlite::string},
		{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t}} };
	return command;
}

void Command_get_account_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":name", casted_params);
}

// -----------------------------------------------------------------------------------------------

Command_get_blocks_impl::Command_get_blocks_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	if (sqlite3_prepare_v2(m_handle, "SELECT hash, height, type, shares, difficulty, orphan, block_finder, round, block_found_time, mainnet_reward FROM block ORDER BY height LIMIT 100;", -1, &m_stmt, NULL) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(m_handle) << std::endl;
	}
}

std::any Command_get_blocks_impl::get_command() const
{
	Command_type_sqlite command{ m_stmt,
		{{Column_sqlite::string}, 
		{Column_sqlite::int32}, 
		{Column_sqlite::string},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t},
		{Column_sqlite::int32},
		{Column_sqlite::string}, 
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::double_t}} };
	return command;
}
// -----------------------------------------------------------------------------------------------

Command_get_latest_round_impl::Command_get_latest_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT id, round_number, total_shares, total_reward, blocks, connection_count, start_date_time, end_date_time, is_active, is_paid FROM round ORDER BY ID DESC LIMIT 1;", -1, &m_stmt, NULL);
}

std::any Command_get_latest_round_impl::get_command() const
{
	Command_type_sqlite command{ m_stmt,
		{{Column_sqlite::int64},
		{Column_sqlite::int32},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t},
		{Column_sqlite::int32},
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::int32}} };
	return command;
}

// -----------------------------------------------------------------------------------------------

Command_get_payments_impl::Command_get_payments_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT name, amount, payment_date_time FROM payment WHERE name = :name;", -1, &m_stmt, NULL);
}

std::any Command_get_payments_impl::get_command() const
{
	Command_type_sqlite command{ m_stmt,
		{{Column_sqlite::string},
		{Column_sqlite::double_t},
		{Column_sqlite::string}} };
	return command;
}

void Command_get_payments_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":name", casted_params);
}

// -----------------------------------------------------------------------------------------------
// Write commands
// -----------------------------------------------------------------------------------------------
Command_create_db_schema_impl::Command_create_db_schema_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string create_tables{ R"(CREATE TABLE IF NOT EXISTS round (
				  id INTEGER PRIMARY KEY AUTOINCREMENT,
				  round_number INTEGER NOT NULL,
				  total_shares REAL,
				  total_reward REAL,
				  blocks INTEGER,
				  connection_count INTEGER
				);

				CREATE TABLE IF NOT EXISTS block (
				  id INTEGER PRIMARY KEY AUTOINCREMENT,
				  hash TEXT NOT NULL,
				  height INTEGER NOT NULL,
				  type TEXT NOT NULL
				  reward INTEGER NOT NULL,
				  difficulty REAL NOT NULL,
				  orphan INTEGER NOT NULL,
				  block_finder TEXT NOT NULL,
				  round INTEGER NOT NULL,
				  block_found_time TEXT NOT NULL,
				  FOREIGN KEY(round) REFERENCES round(id),
				  FOREIGN KEY(block_finder) REFERENCES account(name)
				);

				CREATE TABLE IF NOT EXISTS account (
				  name TEXT PRIMARY KEY,
				  created_at TEXT NOT NULL,
				  last_active TEXT,
				  connection_count INTEGER,
				  shares REAL,
				  reward REAL,
				  hashrate REAL
				);

				CREATE TABLE IF NOT EXISTS banned_connections_api (
				  id INTEGER PRIMARY KEY AUTOINCREMENT,
				  ip TEXT NOT NULL
				);

				CREATE TABLE IF NOT EXISTS banned_users_connections (
				  user TEXT,
				  ip TEXT,
				  PRIMARY KEY (user, ip)
				);)"
	};

	sqlite3_prepare_v2(m_handle, create_tables.c_str(), -1, &m_stmt, NULL);
}
// -----------------------------------------------------------------------------------------------
Command_create_account_impl::Command_create_account_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string create_account{R"(INSERT INTO account 
		(name, created_at, last_active, connection_count,shares, reward, hashrate) 
		VALUES(:name, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 0, 0, 0, 0)"};

	sqlite3_prepare_v2(m_handle, create_account.c_str(), -1, &m_stmt, NULL);
}

void Command_create_account_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":name", casted_params);
}

// -----------------------------------------------------------------------------------------------
Command_add_payment_impl::Command_add_payment_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string add_payment{ R"(INSERT INTO payment 
		(name, amount, payment_date_time) 
		VALUES(:name, :amount, CURRENT_TIMESTAMP)" };

	sqlite3_prepare_v2(m_handle, add_payment.c_str(), -1, &m_stmt, NULL);
}

void Command_add_payment_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_add_payment_params>(m_params);
	bind_param(m_stmt, ":name", casted_params.m_account);
	bind_param(m_stmt, ":amount", casted_params.m_amount);
}

}
}
}