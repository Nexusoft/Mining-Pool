#include "persistance/sqlite/command/command_impl.hpp"
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

void Command_base_database_sqlite::reset()
{
	sqlite3_reset(m_stmt);
	sqlite3_clear_bindings(m_stmt);
}

// -----------------------------------------------------------------------------------------------

Command_banned_user_and_ip_impl::Command_banned_user_and_ip_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle,
		"SELECT user, ip FROM banned_users_connections WHERE user = :user AND ip = :ip;",
		-1, &m_stmt, NULL);
}

std::any Command_banned_user_and_ip_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt}, {{Column_sqlite::string}, {Column_sqlite::string }} };
	return command;
}

void Command_banned_user_and_ip_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::array<std::string, 2>>(m_params);
	bind_param(m_stmt, ":user", casted_params[0]);
	bind_param(m_stmt, ":ip", casted_params[1]);
}

// -----------------------------------------------------------------------------------------------

Command_banned_api_ip_impl::Command_banned_api_ip_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT ip FROM banned_connections_api WHERE ip = :ip;", -1, &m_stmt, NULL);
}

std::any Command_banned_api_ip_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt}, {{Column_sqlite::string}} };
	return command;
}

void Command_banned_api_ip_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":ip", casted_params);
}

// -----------------------------------------------------------------------------------------------

Command_account_exists_impl::Command_account_exists_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT COUNT(name) FROM account WHERE name = :name;", -1, &m_stmt, NULL);
}

std::any Command_account_exists_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt}, {{Column_sqlite::int32}} };
	return command;
}

void Command_account_exists_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":name", casted_params);
}

// -----------------------------------------------------------------------------------------------
Command_get_account_impl::Command_get_account_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT name, created_at, last_active, connection_count, shares, hashrate, display_name FROM account WHERE name = :name;", -1, &m_stmt, NULL);
}

std::any Command_get_account_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::string},
		{Column_sqlite::string},
		{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t},
		{Column_sqlite::string}} };
	return command;
}

void Command_get_account_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":name", casted_params);
}

// -----------------------------------------------------------------------------------------------

Command_get_blocks_impl::Command_get_blocks_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	if (sqlite3_prepare_v2(m_handle, "SELECT hash, height, type, difficulty, orphan, block_finder, round, block_found_time, mainnet_reward FROM block ORDER BY height LIMIT 100;", -1, &m_stmt, NULL) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(m_handle) << std::endl;
	}
}

std::any Command_get_blocks_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::string}, 
		{Column_sqlite::int32}, 
		{Column_sqlite::string},
		{Column_sqlite::double_t},
		{Column_sqlite::int32},
		{Column_sqlite::string}, 
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::double_t}} };
	return command;
}
// -----------------------------------------------------------------------------------------------

Command_get_latest_round_impl::Command_get_latest_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT round_number, total_shares, total_reward, blocks, start_date_time, end_date_time, is_active, is_paid FROM round ORDER BY round_number DESC LIMIT 1;", -1, &m_stmt, NULL);
}

std::any Command_get_latest_round_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::int64},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t},
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::int32}} };
	return command;
}

// -----------------------------------------------------------------------------------------------

Command_get_round_impl::Command_get_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT round_number, total_shares, total_reward, blocks, start_date_time, end_date_time, is_active, is_paid FROM round WHERE round_number = :round_number;", -1, &m_stmt, NULL);
}

std::any Command_get_round_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::int64},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t},
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::int32}} };
	return command;
}

void Command_get_round_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::int64_t>(m_params);
	bind_param(m_stmt, ":round_number", casted_params);
}

// -----------------------------------------------------------------------------------------------

Command_get_payments_impl::Command_get_payments_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT name, amount, shares, payment_date_time, round, tx_id FROM payment WHERE name = :name;", -1, &m_stmt, NULL);
}

std::any Command_get_payments_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::string},
		{Column_sqlite::double_t},
		{Column_sqlite::double_t},
		{Column_sqlite::string},
		{Column_sqlite::int64},
		{Column_sqlite::string}} };
	return command;
}

void Command_get_payments_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":name", casted_params);
}
// -----------------------------------------------------------------------------------------------

Command_get_config_impl::Command_get_config_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT version, difficulty_divider, fee, mining_mode, round_duration_hours FROM config;", -1, &m_stmt, NULL);
}

std::any Command_get_config_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::int32}} };
	return command;
}

// -----------------------------------------------------------------------------------------------
Command_get_active_accounts_from_round_impl::Command_get_active_accounts_from_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT name, shares FROM account WHERE shares > 0;", -1, &m_stmt, NULL);
}

std::any Command_get_active_accounts_from_round_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::string},
		{Column_sqlite::double_t}} };
	return command;
}

// -----------------------------------------------------------------------------------------------
Command_get_blocks_from_round_impl::Command_get_blocks_from_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT hash, height, type, difficulty, orphan, block_finder, round, block_found_time, mainnet_reward FROM block WHERE round = :round;", -1, &m_stmt, NULL);
}

std::any Command_get_blocks_from_round_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::double_t},
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::int32},
		{Column_sqlite::string},
		{Column_sqlite::double_t}} };
	return command;
}

void Command_get_blocks_from_round_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::int64_t>(m_params);
	bind_param(m_stmt, ":round", casted_params);
}

// -----------------------------------------------------------------------------------------------
Command_get_total_shares_from_accounts_impl::Command_get_total_shares_from_accounts_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT SUM(shares) FROM account", -1, &m_stmt, NULL);
}

std::any Command_get_total_shares_from_accounts_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt}, {{Column_sqlite::double_t}} };
	return command;
}
// -----------------------------------------------------------------------------------------------
Command_get_not_paid_data_from_round_impl::Command_get_not_paid_data_from_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT name, amount, shares, payment_date_time, round, tx_id FROM payment WHERE round = :round AND payment_date_time = ''", -1, &m_stmt, NULL);
}

std::any Command_get_not_paid_data_from_round_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::string},
		 {Column_sqlite::double_t},
		 {Column_sqlite::double_t},
		 {Column_sqlite::string},
		 {Column_sqlite::int64},
		 {Column_sqlite::string}}};
	return command;
}

void Command_get_not_paid_data_from_round_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::int64_t>(m_params);
	bind_param(m_stmt, ":round", casted_params);
}
// -----------------------------------------------------------------------------------------------
Command_get_unpaid_rounds_impl::Command_get_unpaid_rounds_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT round_number FROM round WHERE is_paid = 0 AND is_active = 0", -1, &m_stmt, NULL);
}

std::any Command_get_unpaid_rounds_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt}, {{Column_sqlite::int64}} };
	return command;
}
// -----------------------------------------------------------------------------------------------

Command_get_blocks_without_hash_from_round_impl::Command_get_blocks_without_hash_from_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT height FROM block WHERE hash = '' AND round = :round", -1, &m_stmt, NULL);
}

std::any Command_get_blocks_without_hash_from_round_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt}, {{Column_sqlite::int32}} };
	return command;
}

void Command_get_blocks_without_hash_from_round_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::int64_t>(m_params);
	bind_param(m_stmt, ":round", casted_params);
}

// -----------------------------------------------------------------------------------------------
Command_get_pool_hashrate_impl::Command_get_pool_hashrate_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT SUM(hashrate) FROM account WHERE datetime(last_active) >= datetime('now', '-10 Minute')", -1, &m_stmt, NULL);
}

std::any Command_get_pool_hashrate_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt}, {{Column_sqlite::double_t}} };
	return command;
}

// -----------------------------------------------------------------------------------------------
Command_get_longest_chain_finder_impl::Command_get_longest_chain_finder_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT height, difficulty, block_finder, round, account.display_name FROM block INNER JOIN account ON block.block_finder=account.name ORDER BY difficulty DESC LIMIT 1", -1, &m_stmt, NULL);
}

std::any Command_get_longest_chain_finder_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::int32},
		 {Column_sqlite::double_t},
		 {Column_sqlite::string},
		 {Column_sqlite::int64},
		 {Column_sqlite::string}} };
	return command;
}

// -----------------------------------------------------------------------------------------------
Command_get_top_block_finders_impl::Command_get_top_block_finders_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	sqlite3_prepare_v2(m_handle, "SELECT COUNT(*) as num_blocks, account.display_name FROM block INNER JOIN account ON block.block_finder=account.name GROUP BY block_finder ORDER BY num_blocks DESC LIMIT :limit", -1, &m_stmt, NULL);
}

std::any Command_get_top_block_finders_impl::get_command() const
{
	Command_type_sqlite command{ {m_stmt},
		{{Column_sqlite::int32},
		 {Column_sqlite::string}} };
	return command;
}

void Command_get_top_block_finders_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::int32_t>(m_params);
	bind_param(m_stmt, ":limit", casted_params);
}

// -----------------------------------------------------------------------------------------------
// Write commands
// -----------------------------------------------------------------------------------------------

Command_create_account_impl::Command_create_account_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string create_account{R"(INSERT INTO account 
		(name, created_at, last_active, connection_count, shares, hashrate, display_name) 
		VALUES(:name, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, 0, 0, 0, :display_name))"};

	if (sqlite3_prepare_v2(m_handle, create_account.c_str(), -1, &m_stmt, NULL) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(m_handle) << std::endl;
	}
}

void Command_create_account_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_create_account_params>(m_params);
	bind_param(m_stmt, ":name", casted_params.m_name);
	bind_param(m_stmt, ":display_name", casted_params.m_display_name);
}

// -----------------------------------------------------------------------------------------------
Command_add_payment_impl::Command_add_payment_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string add_payment{ R"(INSERT INTO payment 
		(name, amount, shares, payment_date_time, round, tx_id) 
		VALUES(:name, :amount, :shares, :datetime, :round, :tx_id))" };

	sqlite3_prepare_v2(m_handle, add_payment.c_str(), -1, &m_stmt, NULL);
}

void Command_add_payment_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_add_payment_params>(m_params);
	bind_param(m_stmt, ":name", casted_params.m_account);
	bind_param(m_stmt, ":amount", casted_params.m_amount);
	bind_param(m_stmt, ":shares", casted_params.m_shares);
	bind_param(m_stmt, ":datetime", casted_params.m_payment_datetime);
	bind_param(m_stmt, ":round", casted_params.m_round);
	bind_param(m_stmt, ":tx_id", casted_params.m_tx_id);
}

// -----------------------------------------------------------------------------------------------
Command_create_round_impl::Command_create_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string create_round{ R"(INSERT INTO round 
		(total_shares, total_reward, blocks, start_date_time, end_date_time, is_active, is_paid) 
		VALUES(0, 0, 0, CURRENT_TIMESTAMP, :end_date_time, 1, 0))" };

	sqlite3_prepare_v2(m_handle, create_round.c_str(), -1, &m_stmt, NULL);
}

void Command_create_round_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<std::string>(m_params);
	bind_param(m_stmt, ":end_date_time", casted_params);
}

// -----------------------------------------------------------------------------------------------
Command_update_account_impl::Command_update_account_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string update_account{ R"(UPDATE account SET 
			last_active = :last_active, connection_count = :connection_count, shares = :shares, hashrate = :hashrate, display_name = :display_name
			WHERE name = :name)" };

	sqlite3_prepare_v2(m_handle, update_account.c_str(), -1, &m_stmt, NULL);
}

void Command_update_account_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_update_account_params>(m_params);
	bind_param(m_stmt, ":last_active", casted_params.m_last_active);
	bind_param(m_stmt, ":connection_count", casted_params.m_connection_count);
	bind_param(m_stmt, ":shares", casted_params.m_shares);
	bind_param(m_stmt, ":hashrate", casted_params.m_hashrate);
	bind_param(m_stmt, ":display_name", casted_params.m_display_name);
	bind_param(m_stmt, ":name", casted_params.m_name);
}

// -----------------------------------------------------------------------------------------------
Command_create_config_impl::Command_create_config_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string create_config{ R"(INSERT INTO config 
		(version, difficulty_divider, fee, mining_mode, round_duration_hours) 
		VALUES('1.1', :difficulty_divider, :fee, :mining_mode, :round_duration_hours))" };

	if (sqlite3_prepare_v2(m_handle, create_config.c_str(), -1, &m_stmt, NULL) != SQLITE_OK)
	{
		std::cout << sqlite3_errmsg(m_handle) << std::endl;
	}
}

void Command_create_config_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_config_params>(m_params);
	bind_param(m_stmt, ":difficulty_divider", casted_params.m_difficulty_divider);
	bind_param(m_stmt, ":fee", casted_params.m_fee);
	bind_param(m_stmt, ":mining_mode", casted_params.m_mining_mode);
	bind_param(m_stmt, ":round_duration_hours", casted_params.m_round_duration_hours);
}

// -----------------------------------------------------------------------------------------------
Command_update_config_impl::Command_update_config_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string create_config{ R"(UPDATE config SET 
			difficulty_divider = :difficulty_divider, fee = :fee, mining_mode = :mining_mode, round_duration_hours = :round_duration_hours)" };

	sqlite3_prepare_v2(m_handle, create_config.c_str(), -1, &m_stmt, NULL);
}

void Command_update_config_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_config_params>(m_params);
	bind_param(m_stmt, ":difficulty_divider", casted_params.m_difficulty_divider);
	bind_param(m_stmt, ":fee", casted_params.m_fee);
	bind_param(m_stmt, ":mining_mode", casted_params.m_mining_mode);
	bind_param(m_stmt, ":round_duration_hours", casted_params.m_round_duration_hours);
}
// -----------------------------------------------------------------------------------------------
Command_reset_shares_from_accounts_impl::Command_reset_shares_from_accounts_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string reset_shares{ R"(UPDATE account SET shares = 0)" };

	sqlite3_prepare_v2(m_handle, reset_shares.c_str(), -1, &m_stmt, NULL);
}

// -----------------------------------------------------------------------------------------------
Command_add_block_impl::Command_add_block_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string add_block{ R"(INSERT INTO block 
		(hash, height, type, difficulty, orphan, block_finder, round, block_found_time, mainnet_reward, share_difficulty) 
		VALUES("", :height, :type, :difficulty, :orphan, :block_finder, :round, CURRENT_TIMESTAMP, :mainnet_reward, :share_difficulty))" };

	sqlite3_prepare_v2(m_handle, add_block.c_str(), -1, &m_stmt, NULL);
}

void Command_add_block_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_add_block_params>(m_params);
	bind_param(m_stmt, ":height", casted_params.m_height);
	bind_param(m_stmt, ":type", casted_params.m_type);
	bind_param(m_stmt, ":difficulty", casted_params.m_difficulty);
	bind_param(m_stmt, ":orphan", casted_params.m_orphan);
	bind_param(m_stmt, ":block_finder", casted_params.m_block_finder);
	bind_param(m_stmt, ":round", casted_params.m_round);
	bind_param(m_stmt, ":mainnet_reward", casted_params.m_mainnet_reward);
	bind_param(m_stmt, ":share_difficulty", casted_params.m_mainnet_reward);
}
// -----------------------------------------------------------------------------------------------
Command_update_block_rewards_impl::Command_update_block_rewards_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string update_block_rewards{ R"(UPDATE block SET 
			orphan = :orphan, mainnet_reward = :mainnet_reward WHERE hash = :hash)" };

	sqlite3_prepare_v2(m_handle, update_block_rewards.c_str(), -1, &m_stmt, NULL);
}

void Command_update_block_rewards_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_update_block_reward_params>(m_params);
	bind_param(m_stmt, ":orphan", casted_params.m_orphan);
	bind_param(m_stmt, ":mainnet_reward", casted_params.m_mainnet_reward);
	bind_param(m_stmt, ":hash", casted_params.m_hash);
}
// -----------------------------------------------------------------------------------------------
Command_update_round_impl::Command_update_round_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string update_round{ R"(UPDATE round SET 
			total_shares = :total_shares, 
			total_reward = :total_reward, 
			blocks = :blocks,
			is_active = :is_active, 
			is_paid = :is_paid 
			WHERE round_number = :round_number)" };

	sqlite3_prepare_v2(m_handle, update_round.c_str(), -1, &m_stmt, NULL);
}

void Command_update_round_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_update_round_params>(m_params);
	bind_param(m_stmt, ":total_shares", casted_params.m_total_shares);
	bind_param(m_stmt, ":total_reward", casted_params.m_total_reward);
	bind_param(m_stmt, ":blocks", casted_params.m_blocks);
	bind_param(m_stmt, ":is_active", casted_params.m_is_active);
	bind_param(m_stmt, ":is_paid", casted_params.m_is_paid);
	bind_param(m_stmt, ":round_number", casted_params.m_round_number);
}
// -----------------------------------------------------------------------------------------------
Command_account_paid_impl::Command_account_paid_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string account_paid{ R"(UPDATE payment SET 
			payment_date_time = CURRENT_TIMESTAMP, tx_id = :tx_id
			WHERE round = :round
				AND name = :name)" };

	sqlite3_prepare_v2(m_handle, account_paid.c_str(), -1, &m_stmt, NULL);
}

void Command_account_paid_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_account_paid_params>(m_params);
	bind_param(m_stmt, ":tx_id", casted_params.m_tx_id);
	bind_param(m_stmt, ":round", casted_params.m_round_number);
	bind_param(m_stmt, ":name", casted_params.m_account);
}

// -----------------------------------------------------------------------------------------------
Command_update_block_hash_impl::Command_update_block_hash_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string update_block_hash{ R"(UPDATE block SET hash = :hash WHERE height = :height)" };
	sqlite3_prepare_v2(m_handle, update_block_hash.c_str(), -1, &m_stmt, NULL);
}

void Command_update_block_hash_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_update_block_hash_params>(m_params);
	bind_param(m_stmt, ":hash", casted_params.m_hash);
	bind_param(m_stmt, ":height", casted_params.m_height);
}

// -----------------------------------------------------------------------------------------------
Command_update_reward_of_payment_impl::Command_update_reward_of_payment_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string update_reward_of_payment{ R"(UPDATE payment SET amount = :amount WHERE name = :name AND round = :round)" };
	sqlite3_prepare_v2(m_handle, update_reward_of_payment.c_str(), -1, &m_stmt, NULL);
}

void Command_update_reward_of_payment_impl::set_params(std::any params)
{
	m_params = std::move(params);
	auto casted_params = std::any_cast<Command_update_reward_of_payment_params>(m_params);
	bind_param(m_stmt, ":amount", casted_params.m_amount);
	bind_param(m_stmt, ":name", casted_params.m_name);
	bind_param(m_stmt, ":round", casted_params.m_round);
}

// -----------------------------------------------------------------------------------------------
Command_delete_empty_payments_impl::Command_delete_empty_payments_impl(sqlite3* handle)
	: Command_base_database_sqlite{ handle }
{
	std::string delete_empty_payments{ R"(DELETE FROM payment WHERE amount = 0 AND tx_id = '')" };
	sqlite3_prepare_v2(m_handle, delete_empty_payments.c_str(), -1, &m_stmt, NULL);
}



}
}
}