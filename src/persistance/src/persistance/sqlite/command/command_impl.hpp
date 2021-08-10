#ifndef NEXUSPOOL_PERSISTANCE_SQLITE_COMMAND_COMMAND_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_SQLITE_COMMAND_COMMAND_IMPL_HPP

#include "persistance/command/command.hpp"
#include "persistance/sqlite/types.hpp"
#include "sqlite/sqlite3.h"
#include <memory>
#include <string>
#include <vector>
#include <variant>
#include <iostream>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {
namespace command {

class Command_base_database_sqlite : public Command_base_database
{
public:
	virtual ~Command_base_database_sqlite() { sqlite3_finalize(m_stmt); }
	Command_base_database_sqlite(sqlite3* handle) : m_handle{ handle }, m_stmt{ nullptr } {}
	Class get_class() const override { return Class::database_sqlite; }
	std::any get_command() const override { return Command_type_sqlite{ m_stmt }; }
	void reset() override;

protected:

	void bind_param(sqlite3_stmt* stmt, const char* name, std::string const& value);
	void bind_param(sqlite3_stmt* stmt, const char* name, int value);
	void bind_param(sqlite3_stmt* stmt, const char* name, int64_t value);
	void bind_param(sqlite3_stmt* stmt, const char* name, double value);

	sqlite3* m_handle;
	sqlite3_stmt* m_stmt;

};


class Command_banned_user_and_ip_impl : public Command_base_database_sqlite
{
public:

	explicit Command_banned_user_and_ip_impl(sqlite3* handle);

	std::any get_command() const override;
	Type get_type() const override { return Type::get_banned_user_and_ip; }
	void set_params(std::any params) override;
};

class Command_banned_api_ip_impl : public Command_base_database_sqlite
{
public:

	explicit Command_banned_api_ip_impl(sqlite3* handle);

	std::any get_command() const override;
	Type get_type() const override { return Type::get_banned_api_ip; }
	void set_params(std::any params) override;
};

class Command_account_exists_impl : public Command_base_database_sqlite
{
public:

	explicit Command_account_exists_impl(sqlite3* handle);

	std::any get_command() const override;
	Type get_type() const override { return Type::account_exists; }
	void set_params(std::any params) override;
};

class Command_get_account_impl : public Command_base_database_sqlite
{
public:

	explicit Command_get_account_impl(sqlite3* handle);

	std::any get_command() const override;
	Type get_type() const override { return Type::get_account; }
	void set_params(std::any params) override;
};

class Command_get_blocks_impl : public Command_base_database_sqlite
{
public:

	explicit Command_get_blocks_impl(sqlite3* handle);

	std::any get_command() const override;
	Type get_type() const override { return Type::get_blocks; }
};

// ------------------------------------------------------------------------------------
// Write commands

class Command_create_db_schema_impl : public Command_base_database_sqlite
{
public:

	explicit Command_create_db_schema_impl(sqlite3* handle);

	Type get_type() const override { return Type::create_db_schema; }
};

class Command_create_account_impl : public Command_base_database_sqlite
{
public:

	explicit Command_create_account_impl(sqlite3* handle);

	Type get_type() const override { return Type::create_account; }
	void set_params(std::any params) override;
};



}
}
}

#endif 
