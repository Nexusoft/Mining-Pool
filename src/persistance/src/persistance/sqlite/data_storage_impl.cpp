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
	, m_db_name{ std::move(db_name) }
{
}

bool Data_storage_impl::execute_command(std::any command)
{
	auto casted_command = std::any_cast<std::shared_ptr<command::Command>>(command);
	command::Result_sqlite result;
	auto return_value = exec_statement_with_result(std::any_cast<command::Command_type_sqlite>(casted_command->get_command()), result);
	casted_command->set_result(std::move(result));
	return return_value;
}

bool Data_storage_impl::exec_statement_with_result(command::Command_type_sqlite sql_command, command::Result_sqlite& result)
{
	int ret = 0;
	while ((ret = sqlite3_step(sql_command.m_statement)) == SQLITE_ROW)
	{	
		// process row here
		auto column_index = 0U;
		command::Result_sqlite::Result_columns columns;
		for (auto& column : sql_command.m_columns)
		{
			command::Column_sqlite column;
			switch (column.m_type)
			{
			case command::Column_sqlite::string:
			{
				column.m_data = std::string(reinterpret_cast<const char*>(sqlite3_column_text(sql_command.m_statement, column_index)));
				column.m_type = command::Column_sqlite::string;
				break;
			}

			case command::Column_sqlite::int32:
			{
				column.m_data = sqlite3_column_int(sql_command.m_statement, column_index);
				column.m_type = command::Column_sqlite::int32;
				break;
			}
			case command::Column_sqlite::int64:
			{
				column.m_data = sqlite3_column_int64(sql_command.m_statement, column_index);
				column.m_type = command::Column_sqlite::int64;
				break;
			}
			case command::Column_sqlite::double_t:
			{
				column.m_data = sqlite3_column_double(sql_command.m_statement, column_index);
				column.m_type = command::Column_sqlite::double_t;
				break;
			}
			}
			columns.push_back(column);
			column_index++;
		}
		result.m_rows.push_back(std::move(columns));
	}
	if (ret != SQLITE_DONE) 
	{
		m_logger->error("Sqlite step failure");
		return false;
	}
	return true;
}


bool Data_storage_impl::init()
{
	/*
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
	*/
return true;
}

}
}
}