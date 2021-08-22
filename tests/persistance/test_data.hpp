#ifndef TESTS_PERSISTANCE_TEST_DATA_HPP
#define TESTS_PERSISTANCE_TEST_DATA_HPP

#include <string>
#include <vector>
#include <fstream>
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
	}

	~Test_data()
	{
		sqlite3_close(m_handle);
		sqlite3_shutdown();
	}

	std::string m_db_filename{ "test.sqlite3" };
	std::vector<std::string> const m_invalid_input{ "", "asfagsgdsdfg", "123415234" };
	std::vector<std::string> m_valid_account_names_input{};
	std::vector<std::string> m_banned_connections_api_input{};
	std::vector<std::pair<std::string, std::string>> m_banned_users_connections_input{};

private:

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

	

	sqlite3* m_handle;


};

}
#endif