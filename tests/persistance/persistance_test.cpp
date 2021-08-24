#include <gtest/gtest.h>
#include <sqlite/sqlite3.h>
#include "persistance_fixture.hpp"
#include "persistance/command/command.hpp"
#include <string>

using namespace persistance;
using ::nexuspool::persistance::command::Type;

TEST(Persistance, initialisation)
{
	auto logger = spdlog::stdout_color_mt("test_logger");
	config::Persistance_config config{};
	auto component = persistance::create_component(logger, config);
	EXPECT_TRUE(component);
}

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

// Testdata currently missing
/*
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
*/

// -----------------------------------------------------------------------------------------------
// Write commands
// -----------------------------------------------------------------------------------------------

TEST_F(Persistance_fixture, command_create_account)
{
	std::string account_name{ "testaccount" };
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->create_account(account_name);
	EXPECT_TRUE(result);

	// try to create the account a second time
	result = data_writer->create_account(account_name);
	EXPECT_FALSE(result);

	// get the new account
	auto account_result = m_persistance_component->get_data_reader_factory()->create_data_reader()->get_account(account_name);
	EXPECT_EQ(account_result.m_address, account_name);

	// cleanup db
	m_test_data.delete_from_account_table(account_name);
}

TEST_F(Persistance_fixture, command_add_payment)
{
	std::string account_name{ "testaccount" };
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->add_payment(account_name, 1000.0);
	EXPECT_TRUE(result);

	// add another payment for this account
	result = data_writer->add_payment(account_name, 5000.0);
	EXPECT_TRUE(result);

	// cleanup db
	m_test_data.delete_from_payment_table(account_name);

}

TEST_F(Persistance_fixture, command_create_round)
{
	int round_number = 100;
	auto data_writer = m_persistance_component->get_data_writer_factory()->create_shared_data_writer();
	auto result = data_writer->create_round(round_number);
	EXPECT_TRUE(result);

	// cleanup db
	m_test_data.delete_from_round_table(round_number);

}




