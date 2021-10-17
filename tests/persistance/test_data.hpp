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
		get_valid_blocks_in_round();
	}

	~Test_data()
	{
		sqlite3_close(m_handle);
		sqlite3_shutdown();
	}

	void delete_from_account_table(std::string account)
	{
		delete_from_table("account", "name", std::move(account));
	}

	void delete_from_payment_table(std::string account)
	{
		delete_from_table("payment", "name", std::move(account));
	}

	void delete_from_block_table(std::int64_t block_height)
	{
		delete_from_table("block", "height", block_height);
	}

	void delete_latest_round()
	{
		auto round_number = get_latest_record_id_from_table("round", "round_number");
		delete_from_table("round", "round_number", round_number);
	}

	void delete_from_config_table()
	{
		auto config_id = get_latest_record_id_from_table("config", "id");
		delete_from_table("config", "id", config_id);
	}

	std::string m_db_filename{ "test.sqlite3" };
	std::vector<std::string> const m_invalid_input{ "", "asfagsgdsdfg", "123415234" };
	std::vector<std::string> m_valid_account_names_input{};
	std::vector<std::string> m_banned_connections_api_input{};
	std::vector<std::pair<std::string, std::string>> m_banned_users_connections_input{};
	std::vector<std::int64_t> m_valid_round_numbers_input{};
	std::vector<std::string> m_valid_blocks_in_round_input{};

protected:

	void delete_from_table(std::string table, std::string column_name, std::string column_value)
	{
		std::string column_bind_param_name = ":" + column_name;
		std::string sql_stmt{ "DELETE FROM " };
		sql_stmt += (table + " WHERE " + column_name + " = " + column_bind_param_name);
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, sql_stmt.c_str(), -1, &stmt, 0);
		int index = sqlite3_bind_parameter_index(stmt, column_bind_param_name.c_str());
		if (index == 0)
		{
			return;
		}
		sqlite3_bind_text(stmt, index, column_value.c_str(), -1, SQLITE_TRANSIENT);
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

	void get_valid_blocks_in_round()
	{
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, "SELECT hash FROM block WHERE round = :round;", -1, &stmt, 0);
		int index = sqlite3_bind_parameter_index(stmt, ":round");
		if (index == 0)
		{
			return;
		}
		sqlite3_bind_int64(stmt, index, m_valid_round_numbers_input.front());
		int ret;
		while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			m_valid_blocks_in_round_input.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
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

	std::int64_t get_latest_record_id_from_table(std::string table, std::string column)
	{
		std::string sql_stmt{ "SELECT " };
		sql_stmt += (column + " FROM " + table + " ORDER BY " + column + " DESC LIMIT 1;");
		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(m_handle, sql_stmt.c_str(), -1, &stmt, 0);
		int ret;
		while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			return sqlite3_column_int64(stmt, 0);
		}

		return 0;
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