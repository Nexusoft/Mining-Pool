#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_TYPES_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_TYPES_HPP

#include <cstdint>

namespace nexuspool {
namespace persistance {
namespace command {

// Classes of a command. Generic will often do nothing, its more kind of a base class
enum class Class : std::uint8_t
{
	generic = 0,
	database,
	database_sqlite,
	lld
};

// List of available commands
enum class Type : std::uint8_t
{
	get_banned_user_and_ip = 0,
	get_banned_api_ip,
	account_exists,
	get_account,
	get_active_accounts_from_round,
	get_blocks,
	create_account,
	get_latest_round,
	get_round,
	get_payments,
	add_payment,
	create_round,
	update_account,
	create_config,
	get_config,
	update_config,
	reset_shares_from_accounts,
	add_block,
	update_block_rewards,
	get_blocks_from_round,
	update_round,
	get_total_shares_from_accounts,
	get_not_paid_data_from_round,
	account_paid,
	get_unpaid_rounds,
	update_block_hash,
	get_blocks_without_hash_from_round,
	update_reward_of_payment,
	delete_empty_payments
};



}
}
}

#endif 
