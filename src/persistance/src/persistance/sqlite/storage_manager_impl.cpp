#include "persistance/sqlite/storage_manager_impl.hpp"
#include <spdlog/spdlog.h>
#include <sqlite/sqlite3.h>

namespace nexuspool {
namespace persistance {

Storage_manager_sqlite::Storage_manager_sqlite(std::shared_ptr<spdlog::logger> logger, std::string db_name, bool readonly)
    : m_logger{ std::move(logger) }
    , m_db_name{ std::move(db_name) }
    , m_readonly{readonly}
{
}

void Storage_manager_sqlite::start()
{
    sqlite3_initialize();
    auto result = sqlite3_open_v2(m_db_name.c_str(), &m_handle, m_readonly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (result)
    {
        m_logger->critical("Can't open database: {}", sqlite3_errmsg(m_handle));
        std::exit(1);   // cant do anything without a db
    }

    // config the db -> WAL mode, MT etc
    sqlite3_exec(m_handle, "pragma journal_mode = WAL", NULL, NULL, NULL);

    // create tables
    sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS round (
		  round_number INTEGER PRIMARY KEY AUTOINCREMENT,
		  total_shares REAL,
		  total_reward REAL,
		  blocks INTEGER,
		  start_date_time TEXT NOT NULL,
		  end_date_time TEXT NOT NULL,
		  is_active INTEGER NOT NULL,
		  is_paid INTEGER NOT NULL
		);)", NULL, NULL, NULL);

	sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS block (
		  id INTEGER PRIMARY KEY AUTOINCREMENT,
		  hash TEXT NOT NULL,
		  height INTEGER NOT NULL,
		  type TEXT NOT NULL,
		  difficulty REAL NOT NULL,
		  orphan INTEGER NOT NULL,
		  block_finder TEXT NOT NULL,
		  round INTEGER NOT NULL,
		  block_found_time TEXT NOT NULL,
		  mainnet_reward REAL NOT NULL,
		  share_difficulty REAL,
		  FOREIGN KEY(round) REFERENCES round(round_number),
		  FOREIGN KEY(block_finder) REFERENCES account(name)
		);)", NULL, NULL, NULL);

	sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS account (
		  name TEXT PRIMARY KEY,
		  created_at TEXT NOT NULL,
		  last_active TEXT,
		  connection_count INTEGER,
		  shares REAL,
		  hashrate REAL,
		  display_name TEXT
		);)", NULL, NULL, NULL);

	sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS miner(
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT NOT NULL,
		last_active TEXT,
		shares REAL,
		hashrate REAL,
		display_name TEXT,
		FOREIGN KEY(name) REFERENCES account(name)
		);)", NULL, NULL, NULL);

	sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS payment (
		  id INTEGER PRIMARY KEY AUTOINCREMENT, 
		  name TEXT NOT NULL,
		  amount REAL,
		  shares REAL,
		  payment_date_time TEXT,
		  round INTEGER NOT NULL,
		  tx_id TEXT,
		  FOREIGN KEY(round) REFERENCES round(round_number),
		  FOREIGN KEY(name) REFERENCES account(name)
		);)", NULL, NULL, NULL);

	sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS banned_connections_api (
		  id INTEGER PRIMARY KEY AUTOINCREMENT,
		  ip TEXT NOT NULL
		);)", NULL, NULL, NULL);

	sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS banned_users_connections (
		  user TEXT,
		  ip TEXT,
		  PRIMARY KEY (user, ip)
		);)", NULL, NULL, NULL);

	sqlite3_exec(m_handle, R"(CREATE TABLE IF NOT EXISTS config (
		  id INTEGER PRIMARY KEY AUTOINCREMENT,
		  version TEXT NOT NULL,
		  difficulty_divider INTEGER NOT NULL,
		  fee INTEGER NOT NULL,
		  mining_mode TEXT NOT NULL,
		  round_duration_hours INTEGER NOT NULL
		);)", NULL, NULL, NULL);

	update_db_schema();
}

void Storage_manager_sqlite::stop()
{
    sqlite3_close(m_handle);
    sqlite3_shutdown();
}

void Storage_manager_sqlite::update_db_schema()
{
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(m_handle, "SELECT version FROM config;", -1, &stmt, 0);
	int ret;
	std::string version;
	while ((ret = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		version = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
	}

	if (version == "1.0")
	{
		m_logger->info("Updating DB schema to version 1.1");
		sqlite3_exec(m_handle, R"(ALTER TABLE block ADD COLUMN share_difficulty REAL;)", NULL, NULL, NULL);
		sqlite3_exec(m_handle, R"(UPDATE config SET version = '1.1';)", NULL, NULL, NULL);
	}
	else if (version == "1.1")
	{
		// Bump version number
		m_logger->info("Updating DB schema to version 2.0");
		sqlite3_exec(m_handle, R"(UPDATE config SET version = '2.0';)", NULL, NULL, NULL);
	}
}

}
}
