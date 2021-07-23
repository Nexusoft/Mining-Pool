#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_IMPL_HPP

#include "persistance/command/command.hpp"
#include "sqlite/sqlite3.h"
#include <memory>
#include <string>
#include <iostream>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {
namespace command {

class Command_base_database_sqlite : public Command_base_database
{
public:
	virtual ~Command_base_database_sqlite() = default;
	Command_base_database_sqlite(sqlite3* handle) : m_handle{handle} {}
	Class get_class() const override { return Class::database_sqlite; }

protected:

	void bind_param(sqlite3_stmt* stmt, const char* name, std::string const& value);
	void bind_param(sqlite3_stmt* stmt, const char* name, int value);
	void bind_param(sqlite3_stmt* stmt, const char* name, int64_t value);
	void bind_param(sqlite3_stmt* stmt, const char* name, double value);

	sqlite3* m_handle;

};


class Command_banned_user_and_ip_impl : public Command_base_database_sqlite
{
public:

	Command_banned_user_and_ip_impl(sqlite3* handle)
		: Command_base_database_sqlite{handle}
		, m_banned_user_and_ip_stmt{nullptr}
	{
		sqlite3_prepare_v2(m_handle,
			"SELECT * FROM banned_users_connections WHERE user = ':user' AND ip = ':ip';",
			-1, &m_banned_user_and_ip_stmt, NULL);
	}

	~Command_banned_user_and_ip_impl() { sqlite3_finalize(m_banned_user_and_ip_stmt); }
	std::any get_command() const override { return m_banned_user_and_ip_stmt; }
	Type get_type() const override { return Type::get_banned_user_and_ip; }
	void set_params(std::any params) override;

private:
	sqlite3_stmt* m_banned_user_and_ip_stmt;

};

class Command_banned_api_ip_impl : public Command_base_database_sqlite
{
public:

	Command_banned_api_ip_impl(sqlite3* handle)
		: Command_base_database_sqlite{ handle }
		, m_banned_api_ip_stmt{ nullptr }
	{
		if (sqlite3_prepare_v2(m_handle, "SELECT * FROM banned_connections_api WHERE ip = ':ip';", -1, &m_banned_api_ip_stmt, NULL) != SQLITE_OK)
		{
			auto message = sqlite3_errmsg(m_handle);
			std::cout << message << std::endl;
		}
	}

	~Command_banned_api_ip_impl() { sqlite3_finalize(m_banned_api_ip_stmt); }
	std::any get_command() const override { return m_banned_api_ip_stmt; }
	Type get_type() const override { return Type::get_banned_api_ip; }
	void set_params(std::any params) override;

private:
	sqlite3_stmt* m_banned_api_ip_stmt;

};


class Command_create_db_schema_impl : public Command_base_database_sqlite
{
public:

	Command_create_db_schema_impl(sqlite3* handle)
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

		if(sqlite3_prepare_v2(m_handle, create_tables.c_str(), -1, &m_create_tables_stmt, NULL) != SQLITE_OK)
		{
			auto message = sqlite3_errmsg(m_handle);
			std::cout << message << std::endl;
		}
	}

	~Command_create_db_schema_impl() { sqlite3_finalize(m_create_tables_stmt); }
	std::any get_command() const override { return m_create_tables_stmt; }
	Type get_type() const override { return Type::create_db_schema; }

private:

	sqlite3_stmt* m_create_tables_stmt;


};


}
}
}

#endif 
