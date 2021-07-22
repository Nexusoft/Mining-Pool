#include "persistance/sqlite/data_storage_impl.hpp"
#include "persistance/types.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool
{
namespace persistance
{
namespace sqlite
{
Data_storage_impl::Data_storage_impl(std::shared_ptr<spdlog::logger> logger, std::string db_name)
	: m_logger{std::move(logger)}
	, m_init_complete{ false }
	, m_db_name{ std::move(db_name) }
	, m_handle{ nullptr }
{
}

bool Data_storage_impl::execute_command(std::any command)
{
	return true;
}


void Data_storage_impl::exec_statement(sqlite3_stmt* stmt)
{
	if (!m_init_complete)
		return;

	int ret = sqlite3_step(m_update_pool_data_stmt);
	while (ret == SQLITE_BUSY)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ret = sqlite3_step(m_update_pool_data_stmt);
	}
}

bool Data_storage_impl::run_simple_query(std::string query)
{
	sqlite3_stmt* stmt = NULL;
	int rc = sqlite3_prepare_v2(m_handle, "CREATE TABLE tbl ( str TEXT )", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		return false;
	}

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE)
	{
		return false;
	}

	sqlite3_finalize(stmt);

	return true;
}

void Data_storage_impl::bind_param(sqlite3_stmt* stmt, const char* name, std::string const& value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		m_logger->error("Database: bind_param index not found, Name: {}", name);
		return;
	}

	sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

void Data_storage_impl::bind_param(sqlite3_stmt* stmt, const char* name, int value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		m_logger->error("Database: bind_param index not found, Name: {}", name);
		return;
	}

	sqlite3_bind_int(stmt, index, value);
}

void Data_storage_impl::bind_param(sqlite3_stmt* stmt, const char* name, int64_t value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		m_logger->error("Database: bind_param index not found, Name: {}", name);
		return;
	}

	sqlite3_bind_int64(stmt, index, value);
}

void Data_storage_impl::bind_param(sqlite3_stmt* stmt, const char* name, double value)
{
	int index = sqlite3_bind_parameter_index(stmt, name);
	if (index == 0)
	{
		m_logger->error("Database: bind_param index not found, Name: {}", name);
		return;
	}

	sqlite3_bind_double(stmt, index, value);
}

bool Data_storage_impl::init()
{
	char* error_msg = nullptr;
	int result;

	if (!run_simple_query("CREATE DATABASE IF NOT EXISTS nxspool;"))
		return false;
	// if (!run_simple_query("USE nxspool;"))
	//  return false;
	if (!run_simple_query("CREATE TABLE IF NOT EXISTS pool_data\
	(id INTEGER PRIMARY KEY AUTOINCREMENT, \
		round_number INTEGER NOT NULL,\
		block_number INTEGER DEFAULT NULL,\
		round_reward INTEGER DEFAULT NULL,\
		total_shares INTEGER DEFAULT NULL,\
		connection_count INTEGER DEFAULT NULL);"))
		return false;
	if (!run_simple_query("CREATE TABLE IF NOT EXISTS round_history\
	( round_number INTEGER PRIMARY KEY,\
		block_number INTEGER DEFAULT NULL,\
		block_hash TEXT NOT NULL,\
		round_reward INTEGER DEFAULT NULL,\
		total_shares INTEGER DEFAULT NULL,\
		block_finder TEXT NOT NULL,\
		orphan INTEGER DEFAULT NULL,\
		block_found_time TEXT NOT NULL);"))
		return false;
	if (!run_simple_query("CREATE TABLE IF NOT EXISTS connection_history\
	( account_address TEXT NOT NULL,\
		series_time TEXT NOT NULL,\
		last_save_time TEXT NOT NULL,\
		connection_count INTEGER DEFAULT NULL,\
		pps REAL DEFAULT NULL,\
		wps REAL DEFAULT NULL,\
		PRIMARY KEY (account_address, series_time));"))
		return false;
	if (!run_simple_query("CREATE TABLE IF NOT EXISTS account_data\
	( account_address TEXT PRIMARY KEY,\
		last_save_time TEXT NOT NULL,\
		connection_count INTEGER DEFAULT NULL,\
		round_shares INTEGER DEFAULT NULL,\
		balance INTEGER DEFAULT NULL,\
		pending_payout INTEGER DEFAULT NULL);"))
		return false;
	if (!run_simple_query("CREATE TABLE IF NOT EXISTS earnings_history\
	( account_address TEXT PRIMARY KEY,\
		round_number INTEGER NOT NULL ,\
		block_number INTEGER DEFAULT NULL,\
		round_shares INTEGER DEFAULT NULL,\
		amount_earned INTEGER DEFAULT NULL,\
		datetime TEXT NOT NULL);"))
		return false;
	if (!run_simple_query("CREATE INDEX idx_earnings_history ON earnings_history\
		(account_address, round_number);"))
		return false;
	if (!run_simple_query("CREATE TABLE IF NOT EXISTS payment_history\
	( account_address TEXT PRIMARY KEY,\
		round_number INTEGER NOT NULL ,\
		block_number INTEGER DEFAULT NULL,\
		amount_paid INTEGER DEFAULT NULL,\
		datetime TEXT NOT NULL);"))
		return false;
	if (!run_simple_query("CREATE INDEX idx_payment_history ON payment_history\
		(account_address, block_number);"))
		return false;

	// create prepared statements
	int ret = sqlite3_prepare_v2(m_handle,
		"REPLACE INTO nxspool.pool_data VALUES (:round, :blocknumber, "
		":roundreward, :totalshares, :connectioncount);",
		-1, &m_update_pool_data_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle,
		"REPLACE INTO nxspool.connection_history VALUES (':address', "
		"':seriestime', ':time', :connectioncount, :pps, :wps);",
		-1, &m_save_connection_data_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle,
		"INSERT INTO nxspool.round_history VALUES (:round, :blocknumber,\
		':hashblock', :roundrewards, :totalshares, ':blockfinder', :orphan, ':blockfoundtime');",
		-1, &m_add_round_data_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle,
		"UPDATE nxspool.round_history SET orphan = 1 WHERE "
		"round_number = :round;",
		-1, &m_flag_round_as_orphan_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle,
		"INSERT INTO nxspool.account_data (account_address, last_save_time, connection_count, round_shares, balance, pending_payout) "
		"VALUES (':accountaddress', :currenttime, :connections, :shares, :balance, :pendingpayouts) "
		"ON CONFLICT(account_address) DO UPDATE SET last_save_time = excluded.last_save_time, "
		"connection_count = excluded.connection_count, "
		"round_shares = excluded.round_shares, "
		"balance = excluded.balance, "
		"pending_payout = excluded.pending_payout; ", -1, &m_update_account_data_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle, "UPDATE nxspool.account_data SET round_shares = 0 ;",
		-1, &m_clear_account_round_shares_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle,
		"INSERT INTO nxspool.earnings_history VALUES (':accountaddress', :round, :blocknumber, :shares, :amountearned, ':time';",
		-1, &m_add_account_earnings_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle, "DELETE FROM nxspool.earnings_history WHERE account_address=':accountaddress' AND round_number= :round;",
		-1, &m_delete_account_earnings_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle, "INSERT INTO nxspool.payment_history VALUES (':accountaddress', :round, :blocknumber, :amountpaid, ':time');",
		-1, &m_add_account_payment_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	ret = sqlite3_prepare_v2(m_handle, "DELETE FROM nxspool.payment_history WHERE account_address=':accountaddress' AND round_number= :round;",
		-1, &m_delete_account_payment_stmt, NULL);
	if (SQLITE_OK != ret)
	{
		return false;
	}

	m_init_complete = true;
	return true;
}

}
}
}