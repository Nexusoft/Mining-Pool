#ifndef NEXUSPOOL_PERSISTANCE_SQLITE_DATA_STORAGE_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_SQLITE_DATA_STORAGE_IMPL_HPP

#include "persistance/data_storage.hpp"
#include "sqlite/sqlite3.h"
#include <memory>
#include <string>

namespace spdlog { class logger; }

namespace nexuspool {
namespace persistance {
namespace sqlite {

class Data_storage_impl : public Data_storage
{
public:

	Data_storage_impl(std::shared_ptr<spdlog::logger> logger, std::string db_name);

	bool init() override;
	void close() override;
	bool execute_command(std::any& command) override;

private:

	bool run_simple_query(std::string query);
	void exec_statement(sqlite3_stmt* stmt);
	void bind_param(sqlite3_stmt* stmt, const char* name, std::string const& value);
	void bind_param(sqlite3_stmt* stmt, const char* name, int value);
	void bind_param(sqlite3_stmt* stmt, const char* name, int64_t value);
	void bind_param(sqlite3_stmt* stmt, const char* name, double value);

	std::shared_ptr<spdlog::logger> m_logger;
	bool m_init_complete;
	std::string m_db_name;
	sqlite3* m_handle;

	// prepared statements
	sqlite3_stmt* m_update_pool_data_stmt;
	sqlite3_stmt* m_save_connection_data_stmt;
	sqlite3_stmt* m_add_round_data_stmt;
	sqlite3_stmt* m_flag_round_as_orphan_stmt;
	sqlite3_stmt* m_update_account_data_stmt;
	sqlite3_stmt* m_clear_account_round_shares_stmt;
	sqlite3_stmt* m_add_account_earnings_stmt;
	sqlite3_stmt* m_delete_account_earnings_stmt;
	sqlite3_stmt* m_add_account_payment_stmt;
	sqlite3_stmt* m_delete_account_payment_stmt;

};

}
}
}

#endif
