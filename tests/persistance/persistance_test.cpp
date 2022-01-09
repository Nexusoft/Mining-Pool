#include <gtest/gtest.h>
#include <sqlite/sqlite3.h>
#include "persistance_fixture.hpp"
#include "persistance/command/command.hpp"
#include <string>

using namespace persistance;
using ::nexuspool::persistance::command::Type;


/*
get_active_accounts_from_round,
*/

TEST_F(Persistance_fixture, create_shared_data_writer)
{
	auto data_writer_factory = m_persistance_component->get_data_writer_factory();
	EXPECT_TRUE(data_writer_factory);
	auto shared_data_writer = data_writer_factory->create_shared_data_writer();
	EXPECT_EQ(shared_data_writer.use_count(), 2);	// factory stores one shared_ptr and gives one shared_ptr out

	auto shared_data_writer_2 = data_writer_factory->create_shared_data_writer();
	EXPECT_EQ(shared_data_writer_2.use_count(), 3);
}

TEST_F(Persistance_fixture, command_is_user_and_connection_banned)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	for (auto const& invalid_input : m_test_data.m_invalid_input)
	{
		auto result = data_reader->is_user_and_connection_banned(invalid_input, invalid_input);
		EXPECT_FALSE(result);
	}

	for (auto const& valid_input : m_test_data.m_banned_users_connections_input)
	{
		auto result = data_reader->is_user_and_connection_banned(valid_input.first, valid_input.second);
		EXPECT_TRUE(result);
	}
}

TEST_F(Persistance_fixture, command_is_connection_banned)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	for (auto const& invalid_input : m_test_data.m_invalid_input)
	{
		auto result = data_reader->is_connection_banned(invalid_input);
		EXPECT_FALSE(result);
	}

	for (auto const& valid_input : m_test_data.m_banned_connections_api_input)
	{
		auto result = data_reader->is_connection_banned(valid_input);
		EXPECT_TRUE(result);
	}
}

TEST_F(Persistance_fixture, command_account_exists)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	for (auto const& invalid_input : m_test_data.m_invalid_input)
	{
		auto result = data_reader->does_account_exists(invalid_input);
		EXPECT_FALSE(result);
	}

	for (auto const& valid_input : m_test_data.m_valid_account_names_input)
	{
		auto result = data_reader->does_account_exists(valid_input);
		EXPECT_TRUE(result);
	}
}

TEST_F(Persistance_fixture, command_get_account)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	for (auto const& valid_input : m_test_data.m_valid_account_names_input)
	{
		auto result = data_reader->get_account(valid_input);
		EXPECT_EQ(result.m_address, valid_input);
	}
}

TEST_F(Persistance_fixture, command_get_total_shares_from_accounts)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_reader->get_total_shares_from_accounts();
	EXPECT_GE(result, 0);
}

TEST_F(Persistance_fixture, command_get_pool_hashrate)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_reader->get_pool_hashrate();
	EXPECT_GE(result, 0);
}

TEST_F(Persistance_fixture, command_get_latest_blocks)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_reader->get_latest_blocks();
	EXPECT_FALSE(result.empty());

}

TEST_F(Persistance_fixture, command_get_latest_round)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_reader->get_latest_round();
	EXPECT_TRUE(result.m_round);
}

TEST_F(Persistance_fixture, command_get_round)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	for (auto const& valid_input : m_test_data.m_valid_round_numbers_input)
	{
		auto result = data_reader->get_round(valid_input);
		EXPECT_TRUE(!result.is_empty());
		EXPECT_EQ(result.m_round, valid_input);
	}
}

TEST_F(Persistance_fixture, command_get_active_accounts_from_round)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_reader->get_active_accounts_from_round();
	EXPECT_FALSE(result.empty());

}

TEST_F(Persistance_fixture, command_get_payments)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	for (auto const& invalid_input : m_test_data.m_invalid_input)
	{
		auto result = data_reader->get_payments(invalid_input);
		EXPECT_TRUE(result.empty());
	}

	for (auto const& valid_input : m_test_data.m_valid_account_names_input)
	{
		auto result = data_reader->get_payments(valid_input);
		EXPECT_FALSE(result.empty());

		for (auto const& payment_result : result)
		{
			EXPECT_EQ(payment_result.m_account, valid_input);
		}
	}
}

TEST_F(Persistance_fixture, command_get_blocks_from_round)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_reader->get_blocks_from_round(m_test_data.m_valid_round_numbers_input.front());
	EXPECT_FALSE(result.empty());

	for (auto i = 0U; i < result.size(); i++)
	{
		EXPECT_EQ(result[i].m_hash, m_test_data.m_valid_blocks_in_round_input[i]);
	}
}

TEST_F(Persistance_fixture, command_get_longest_chain_finder)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_reader->get_longest_chain_finder();
	EXPECT_FALSE(result.is_empty());
}

// -----------------------------------------------------------------------------------------------
// Write commands
// -----------------------------------------------------------------------------------------------

TEST_F(Persistance_fixture, command_create_account)
{
	std::string account_name{ "testaccount" };
	std::string display_name{ "user1" };
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->create_account(account_name, display_name);
	EXPECT_TRUE(result);

	// try to create the account a second time
	result = data_writer->create_account(account_name, display_name);
	EXPECT_FALSE(result);

	// get the new account
	auto account_result = m_persistance_component->get_data_reader_factory()->create_data_reader()->get_account(account_name);
	EXPECT_EQ(account_result.m_address, account_name);
	EXPECT_EQ(account_result.m_display_name, display_name);

	// cleanup db
	m_test_data.delete_from_account_table(account_name);
}


TEST_F(Persistance_fixture, command_update_account)
{
	std::string account_name{ "testaccount" };
	std::string display_name{ "user1" };
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	// create a new testaccount
	auto result_create_account = data_writer->create_account(account_name, "");
	EXPECT_TRUE(result_create_account);

	// Data to update
	Account_data account_data;
	account_data.m_hashrate = 1000;
	account_data.m_shares = 10000;
	account_data.m_connections = 1;
	account_data.m_address = account_name;
	account_data.m_display_name = display_name;

	auto result_update_account = data_writer->update_account(account_data);
	EXPECT_TRUE(result_update_account);

	// get updated account to compare results
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result_account = data_reader->get_account(account_name);

	EXPECT_EQ(result_account.m_hashrate, account_data.m_hashrate);
	EXPECT_EQ(result_account.m_shares, account_data.m_shares);
	EXPECT_EQ(result_account.m_connections, account_data.m_connections);
	EXPECT_EQ(result_account.m_display_name, account_data.m_display_name);

	// cleanup db
	m_test_data.delete_from_account_table(account_name);

}

TEST_F(Persistance_fixture, command_add_payment)
{
	persistance::Payment_data const payment_input{ "testaccount", 1000.0, 200.0, "", 1 };
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->add_payment(payment_input);
	EXPECT_TRUE(result);

	// add another payment for this account
	result = data_writer->add_payment(payment_input);
	EXPECT_TRUE(result);

	// cleanup db
	m_test_data.delete_from_payment_table(payment_input.m_account);
}

TEST_F(Persistance_fixture, command_create_round)
{
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->create_round("end_datetime");
	EXPECT_TRUE(result);

	// cleanup db
	m_test_data.delete_latest_round();

}

TEST_F(Persistance_fixture, command_update_round)
{
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto result = data_writer->create_round("end_datetime");
	EXPECT_TRUE(result);

	auto result_before_update = data_reader->get_latest_round();
	EXPECT_TRUE(result_before_update.m_round);

	// update round
	result_before_update.m_is_paid = true;
	result_before_update.m_is_active = false;
	result_before_update.m_total_rewards = 100.256;
	result_before_update.m_total_shares = 200;
	result_before_update.m_blocks = 10;

	auto update_round_result = data_writer->update_round(result_before_update);
	EXPECT_TRUE(update_round_result);

	auto result_after_update = data_reader->get_latest_round();
	EXPECT_TRUE(result_after_update.m_round);

	EXPECT_EQ(result_before_update.m_is_paid, result_after_update.m_is_paid);
	EXPECT_EQ(result_before_update.m_is_active, result_after_update.m_is_active);
	EXPECT_EQ(result_before_update.m_total_rewards, result_after_update.m_total_rewards);
	EXPECT_EQ(result_before_update.m_total_shares, result_after_update.m_total_shares);
	EXPECT_EQ(result_before_update.m_blocks, result_after_update.m_blocks);

	// cleanup db
	m_test_data.delete_latest_round();
}

TEST_F(Persistance_fixture, command_get_unpaid_rounds)
{
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto unpaid_results = data_reader->get_unpaid_rounds();
	EXPECT_FALSE(unpaid_results.empty());
}

TEST_F(Persistance_fixture, commands_config)
{
	std::string config_mining_mode_input{ "HASH" };
	int config_fee_input = 3;
	int config_difficulty_divider_input = 4;
	int config_round_duration_hours_input = 24;

	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();

	// get config from empty table
	auto result_get_config = data_reader->get_config();
	EXPECT_TRUE(result_get_config.m_version.empty());

	// insert a config
	auto result = data_writer->create_config(config_mining_mode_input, config_fee_input, config_difficulty_divider_input, config_round_duration_hours_input);
	EXPECT_TRUE(result);

	result_get_config = data_reader->get_config();
	EXPECT_EQ(result_get_config.m_mining_mode, config_mining_mode_input);
	EXPECT_EQ(result_get_config.m_fee, config_fee_input);
	EXPECT_EQ(result_get_config.m_difficulty_divider, config_difficulty_divider_input);
	EXPECT_EQ(result_get_config.m_round_duration_hours, config_round_duration_hours_input);

	// update config
	config_mining_mode_input = "PRIME";
	config_fee_input = 30;
	config_difficulty_divider_input = 40;
	config_round_duration_hours_input = 48;

	result = data_writer->update_config(config_mining_mode_input, config_fee_input, config_difficulty_divider_input, config_round_duration_hours_input);
	EXPECT_TRUE(result);

	result_get_config = data_reader->get_config();
	EXPECT_EQ(result_get_config.m_mining_mode, config_mining_mode_input);
	EXPECT_EQ(result_get_config.m_fee, config_fee_input);
	EXPECT_EQ(result_get_config.m_difficulty_divider, config_difficulty_divider_input);
	EXPECT_EQ(result_get_config.m_round_duration_hours, config_round_duration_hours_input);

	// cleanup db
	m_test_data.delete_from_config_table();
}

TEST_F(Persistance_fixture, command_add_block)
{
	std::int64_t const block_height_input = 5983133;
	persistance::Block_data const block_input{"", static_cast<std::uint32_t>(block_height_input), "HASH", 7896, false, "blockfinder", 5, "current_datetime", 2.54};
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->add_block(block_input);
	EXPECT_TRUE(result);

	// cleanup db
	m_test_data.delete_from_block_table(block_height_input);
}

TEST_F(Persistance_fixture, command_update_block_hash)
{
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	std::uint32_t const round_input = 999;
	std::int64_t const block_height_input_1 = 5983133;
	std::int64_t const block_height_input_2 = 5983134;
	std::string const block_hash_input_1 = "blockhash1";
	std::string const block_hash_input_2 = "blockhash2";

	// add 2 blocks without hash
	persistance::Block_data const block_input_1{ "", static_cast<std::uint32_t>(block_height_input_1), "HASH", 7896, false, "blockfinder", round_input, "current_datetime", 2.54 };
	auto result = data_writer->add_block(block_input_1);
	EXPECT_TRUE(result);

	persistance::Block_data const block_input_2{ "", static_cast<std::uint32_t>(block_height_input_2), "HASH", 6895, false, "blockfinder", round_input, "current_datetime", 2.64 };
	result = data_writer->add_block(block_input_2);
	EXPECT_TRUE(result);

	// get the 2 added blocks without hash
	auto heights = data_reader->get_blocks_without_hash_from_round(round_input);
	EXPECT_TRUE(!heights.empty());

	// update the hash of the 2 blocks
	result = data_writer->update_block_hash(heights[0], block_hash_input_1);
	EXPECT_TRUE(result);
	result = data_writer->update_block_hash(heights[1], block_hash_input_2);
	EXPECT_TRUE(result);

	// compare the blocks
	auto blocks = data_reader->get_blocks_from_round(round_input);
	EXPECT_TRUE(!blocks.empty());

	EXPECT_EQ(blocks[0].m_hash, block_hash_input_1);
	EXPECT_EQ(blocks[1].m_hash, block_hash_input_2);


	// cleanup db
	m_test_data.delete_from_block_table(block_height_input_1);
	m_test_data.delete_from_block_table(block_height_input_2);
}

TEST_F(Persistance_fixture, command_update_block_rewards)
{
	std::int64_t const block_height_input = 5983133;
	std::string const block_hash_input_1 = "blockhash1";
	persistance::Block_data const block_input{ "", block_height_input, "HASH", 7896, false, "blockfinder", 5, "current_datetime", 2.54 };
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->add_block(block_input);
	EXPECT_TRUE(result);

	// update block_hash
	result = data_writer->update_block_hash(block_height_input, block_hash_input_1);
	EXPECT_TRUE(result);

	// update rewards
	result = data_writer->update_block_rewards(block_hash_input_1, true, 5.0);
	EXPECT_TRUE(result);

	// cleanup db
	m_test_data.delete_from_block_table(block_height_input);
}

TEST_F(Persistance_fixture, command_account_paid)
{
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();

	// add a new payment record which is not paid yet (no datetime set)
	std::int64_t const round_number_input{ 500 };
	std::string const account_input{ "testaccount" };
	std::string const tx_id_input{ "test_tx_id" };
	persistance::Payment_data const payment_input{ account_input, 1000.0, 200.0, "", round_number_input };
	auto result = data_writer->add_payment(payment_input);
	EXPECT_TRUE(result);

	// add another payment for this account
	result = data_writer->add_payment(payment_input);
	EXPECT_TRUE(result);

	auto result_not_paid_payments = data_reader->get_not_paid_data_from_round(round_number_input);
	EXPECT_FALSE(result_not_paid_payments.empty());
	for (auto& result_payment : result_not_paid_payments)
	{
		EXPECT_EQ(result_payment.m_account, account_input);
		EXPECT_EQ(result_payment.m_amount, payment_input.m_amount);
		EXPECT_EQ(result_payment.m_round, payment_input.m_round);
		EXPECT_EQ(result_payment.m_shares, payment_input.m_shares);
		EXPECT_TRUE(result_payment.m_payment_date_time.empty());		// datetime empty = not paid yet
		EXPECT_TRUE(result_payment.m_tx_id.empty());					// tx_id empty = not paid yet
	}

	// pay account
	result = data_writer->account_paid(round_number_input, account_input, tx_id_input);
	EXPECT_TRUE(result);

	// now this command doesn't deliver any results
	result_not_paid_payments = data_reader->get_not_paid_data_from_round(round_number_input);
	EXPECT_TRUE(result_not_paid_payments.empty());

	// get the payments data through other command (for frontend api)
	auto const result_payments = data_reader->get_payments(account_input);
	for (auto& result_payment : result_payments)
	{
		EXPECT_EQ(result_payment.m_account, account_input);
		EXPECT_EQ(result_payment.m_amount, payment_input.m_amount);
		EXPECT_EQ(result_payment.m_round, payment_input.m_round);
		EXPECT_EQ(result_payment.m_shares, payment_input.m_shares);
		EXPECT_FALSE(result_payment.m_payment_date_time.empty());
		EXPECT_FALSE(result_payment.m_tx_id.empty());
	}

	// cleanup db
	m_test_data.delete_from_payment_table(payment_input.m_account);
}

TEST_F(Persistance_fixture, command_delete_empty_payment)
{
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();

	// add a new payment record which is not paid yet (no datetime set)
	std::int64_t const round_number_input{ 500 };
	std::string const account_input{ "testaccount" };
	std::string const tx_id_input{ "test_tx_id" };
	persistance::Payment_data const payment_input{ account_input, 0.0, 200.0, "", round_number_input, ""};
	auto result = data_writer->add_payment(payment_input);
	EXPECT_TRUE(result);

	// add another payment for this account
	result = data_writer->add_payment(payment_input);
	EXPECT_TRUE(result);

	// delelte those not paid data
	result = data_writer->delete_empty_payments();
	EXPECT_TRUE(result);

	// check that there are no unpaid payments in db
	auto result_not_paid_payments = data_reader->get_not_paid_data_from_round(round_number_input);
	EXPECT_TRUE(result_not_paid_payments.empty());
}

TEST_F(Persistance_fixture, command_update_reward_of_payment)
{
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();

	// add a new payment record
	std::int64_t const round_number_input{ 500 };
	std::string const account_input{ "testaccount" };
	double const reward_input{ 40.5 };
	persistance::Payment_data const payment_input{ account_input, 0.0, 200.0, "", round_number_input };
	auto result = data_writer->add_payment(payment_input);
	EXPECT_TRUE(result);

	//update
	result = data_writer->update_reward_of_payment(reward_input, account_input, round_number_input);
	EXPECT_TRUE(result);

	// compare the update
	auto const result_payments = data_reader->get_payments(account_input);
	EXPECT_EQ(result_payments.front().m_amount, reward_input);

	// cleanup db
	m_test_data.delete_from_payment_table(payment_input.m_account);

}

TEST_F(Persistance_fixture, command_reset_shares_from_accounts)
{
	std::string account_name{ "testaccount" };
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto data_reader = m_persistance_component->get_data_reader_factory()->create_data_reader();
	// create a new testaccount
	auto result_create_account = data_writer->create_account(account_name, "");
	EXPECT_TRUE(result_create_account);

	// Data to update
	Account_data account_data;
	account_data.m_hashrate = 1000;
	account_data.m_shares = 10000;
	account_data.m_connections = 1;
	account_data.m_address = account_name;

	auto result_update_account = data_writer->update_account(account_data);
	EXPECT_TRUE(result_update_account);

	// reset shares from accounts
	auto result_shares_reset = data_writer->reset_shares_from_accounts();
	EXPECT_TRUE(result_shares_reset);

	// get updated account to compare shares
	auto result_account = data_reader->get_account(account_name);
	EXPECT_EQ(result_account.m_shares, 0);

	// cleanup db
	m_test_data.delete_from_account_table(account_name);

}







