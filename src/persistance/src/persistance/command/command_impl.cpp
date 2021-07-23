#include "persistance/command/command_impl.hpp"
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

// -----------------------------------------------------------------------------------------------

Command_banned_user_and_ip_impl::Command_banned_user_and_ip_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
	, m_banned_user_and_ip_stmt{ nullptr }
{
	sqlite3_prepare_v2(m_handle,
		"SELECT * FROM banned_users_connections WHERE user = ':user' AND ip = ':ip';",
		-1, &m_banned_user_and_ip_stmt, NULL);
}

std::any Command_banned_user_and_ip_impl::get_command() const
{
	Command_type_sqlite command{ m_banned_user_and_ip_stmt, {{Column_sqlite::string}, {Column_sqlite::string }} };
	return command;
}

void Command_banned_user_and_ip_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::array<std::string, 2>>(m_params);
	bind_param(m_banned_user_and_ip_stmt, ":user", casted_params[0]);
	bind_param(m_banned_user_and_ip_stmt, ":ip", casted_params[1]);
}

// -----------------------------------------------------------------------------------------------

Command_banned_api_ip_impl::Command_banned_api_ip_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
	, m_banned_api_ip_stmt{ nullptr }
{
	sqlite3_prepare_v2(m_handle, "SELECT * FROM banned_connections_api WHERE ip = ':ip';", -1, &m_banned_api_ip_stmt, NULL);
}

std::any Command_banned_api_ip_impl::get_command() const
{
	Command_type_sqlite command{ m_banned_api_ip_stmt, {{Column_sqlite::string}} };
	return command;
}

void Command_banned_api_ip_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_banned_api_ip_stmt, ":ip", casted_params);
}

// -----------------------------------------------------------------------------------------------

Command_create_db_schema_impl::Command_create_db_schema_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
	, m_create_tables_stmt{ nullptr }
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

	sqlite3_prepare_v2(m_handle, create_tables.c_str(), -1, &m_create_tables_stmt, NULL);
}



}
}
}