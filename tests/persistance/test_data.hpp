#ifndef TESTS_PERSISTANCE_TEST_DATA_HPP
#define TESTS_PERSISTANCE_TEST_DATA_HPP

#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <sqlite/sqlite3.h>


namespace
{

class Test_data
{
public:

	Test_data()
	{
		std::ifstream f(m_db_filename);
		if (!f.good())
		{
			system("create_test_sqlite.py");
			system("create_test_data.py");
		}

		sqlite3_initialize();
		sqlite3_open_v2(m_db_filename.c_str(), &m_handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
		get_valid_account_names();
		get_banned_connections_api();
		get_banned_users_connections();
		get_round_numbers();
	}

	~Test_data()
	{
		sqlite3_close(m_handle);
		sqlite3_shutdown();
	}

	void delete_from_account_table(std::string account)
	{
		delete_from_table("account", std::move(account));
	}

	void delete_from_payment_table(std::string account)
	{
		delete_from_table("payment", std::move(account));
	}

	void delete_latest_round()
	{
		auto round_number = get_latest_round_number();
		delete_from_table("round", "round_number", round_number);
	}

	void delete_from_config_table(std::int64_t id)
	{
		delete_from_table("config", "id", id);
	}

	std::string m_db_filename{ "test.sqlite3" };
	std::vector<std::string> const m_invalid_input{ "", "asfagsgdsdfg", "123415234" };
	std::vector<std::string> m_valid_account_names_input{};
	std::vector<std::string> m_banned_connections_api_input{};
	std::vector<std::pair<std::string, std::string>> m_banned_users_connections_input{};
	std::vector<std::int64_t> m_valid_round_numbers_input{};

protected:

	void delete_from_table(std::string table, std::string account)
	{
		std::string sql_stmt{ "DELETE FROM " };
		sql_stmt += (table + " WHERE name = :name");
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, sql_stmt.c_str(), -1, &stmt, 0);
		int index = sqlite3_bind_parameter_index(stmt, ":name");
		if (index == 0)
		{
			return;
		}
		sqlite3_bind_text(stmt, index, account.c_str(), -1, SQLITE_TRANSIENT);
		sqlite3_step(stmt);
	}

	void delete_from_table(std::string table, std::string id_name, std::int64_t id)
	{
		std::string sql_stmt{ "DELETE FROM " };
		sql_stmt += (table + " WHERE " + id_name + " = :id");
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, sql_stmt.c_str(), -1, &stmt, 0);
		int index = sqlite3_bind_parameter_index(stmt, ":id");
		if (index == 0)
		{
			return;
		}
		sqlite3_bind_int64(stmt, index, id);
		sqlite3_step(stmt);
	}

	void get_valid_account_names()
	{
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, "SELECT name FROM account LIMIT 10", -1, &stmt, 0);
		int ret;
		while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			m_valid_account_names_input.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
		}
	}

	void get_banned_connections_api()
	{
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, "SELECT ip FROM banned_connections_api LIMIT 10", -1, &stmt, 0);
		int ret;
		while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			m_banned_connections_api_input.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
		}
	}

	void get_banned_users_connections()
	{
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, "SELECT user, ip FROM banned_users_connections LIMIT 10", -1, &stmt, 0);
		int ret;
		while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			m_banned_users_connections_input.push_back(std::make_pair(
				std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))),
				std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))
				));
		}
	}

	std::int64_t get_latest_round_number()
	{
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, "SELECT round_number FROM round ORDER BY round_number DESC LIMIT 1;", -1, &stmt, 0);
		int ret;
		while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			 return sqlite3_column_int64(stmt, 0);
		}
	}

	void get_round_numbers()
	{
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, "SELECT round_number FROM round;", -1, &stmt, 0);
		int ret;
		while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			m_valid_round_numbers_input.push_back(sqlite3_column_int64(stmt, 0));
		}
	}	

	sqlite3* m_handle;
};

}
#endif