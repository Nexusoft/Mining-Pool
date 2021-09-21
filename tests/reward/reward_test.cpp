#include <gtest/gtest.h>
#include "reward_fixture.hpp"

using namespace ::nexuspool;

TEST_F(Reward_fixture_created_component, difficulty_test)
{
	LLP::CBlock test_block = m_test_data.create_test_block();
	std::string test_block_hash = test_block.GetHash().ToString();

	std::uint32_t test_nbits = test_block.nBits;
	
	auto result = m_component->check_difficulty(test_block, test_nbits);
	EXPECT_EQ(result, nexuspool::reward::Difficulty_result::block_found);

	//change the test block a bit so it will be rejected by the pool
	test_block.nNonce++;
	test_block_hash = test_block.GetHash().ToString();
	result = m_component->check_difficulty(test_block, test_nbits);
	EXPECT_EQ(result, nexuspool::reward::Difficulty_result::reject);

	//todo: find example of a block that would be accpepted by the pool but not mainnet
	uint1024_t test_difficulty_threshold;
	test_difficulty_threshold.SetCompact(test_nbits);
	test_difficulty_threshold *= 2;
	std::uint32_t test_nbits_harder = test_difficulty_threshold.GetCompact();

	result = m_component->check_difficulty(test_block, test_nbits_harder);
}

TEST_F(Reward_fixture_created_component, is_round_active_test)
{
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_latest_round).WillOnce(Return(test_round_data));
	auto result = m_component->is_round_active();
	EXPECT_TRUE(result);
	EXPECT_EQ(m_component->get_current_round(), test_round_data.m_round);
}

TEST_F(Reward_fixture_created_component, is_round_not_active_test)
{
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_latest_round).WillOnce(Return(test_round_not_active_not_paid_data));
	auto result = m_component->is_round_active();
	EXPECT_FALSE(result);
	EXPECT_EQ(m_component->get_current_round(), 0);	// current round is not set if there is no active round
}

TEST_F(Reward_fixture_created_component, start_round_test)
{
	EXPECT_CALL(*m_test_data.m_shared_data_writer_mock, create_round(_)).WillOnce(Return(true));
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_latest_round).WillOnce(Return(test_round_data));

	auto result = m_component->start_round(24);
	EXPECT_TRUE(result);
	EXPECT_EQ(m_component->get_current_round(), test_round_data.m_round);
}

TEST_F(Reward_fixture, create_component_with_active_round)
{
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_latest_round).WillOnce(Return(test_round_data));
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_blocks_from_round(test_current_round)).WillOnce(Return(test_blocks_from_round));

	m_component = reward::create_component(m_logger, std::make_unique<nexus_http_interface::Component_mock>(),
		m_persistance_component_mock->get_data_writer_factory()->create_shared_data_writer(),
		m_persistance_component_mock->get_data_reader_factory()->create_data_reader(),
		"defauöt", "1234");
}

TEST_F(Reward_fixture_created_component, pay_all_with_unknown_round_test)
{
	auto const unknown_round = 100U;
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_round(unknown_round)).WillOnce(Return(persistance::Round_data{}));

	auto result = m_component->pay_all(unknown_round);
	EXPECT_FALSE(result);
}

TEST_F(Reward_fixture_created_component, pay_all_with_active_round_test)
{
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_round(test_round_data.m_round)).WillOnce(Return(test_round_data));

	auto result = m_component->pay_all(test_round_data.m_round);
	EXPECT_FALSE(result);
}

TEST_F(Reward_fixture_created_component, pay_all_with_already_paid_round_test)
{
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_round(test_round_not_active_paid_data.m_round)).WillOnce(Return(test_round_not_active_paid_data));

	auto result = m_component->pay_all(test_round_not_active_paid_data.m_round);
	EXPECT_FALSE(result);
}

TEST_F(Reward_fixture_created_component, pay_all_calculate_rewards_tests)
{
	/*
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_round(test_round_not_active_not_paid_data.m_round)).WillOnce(Return(test_round_not_active_not_paid_data));
	EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_blocks_from_round(test_unpaid_round)).WillOnce(Return(test_blocks_from_unpaid_round));

	for(auto& block : test_blocks_from_unpaid_round)
	{
		common::Block_reward_data reward_data{};
		EXPECT_CALL(*m_test_data.m_http_interface_mock_raw, get_block_reward_data(block.m_hash, _)).WillRepeatedly(Return(true));
		EXPECT_CALL(*m_test_data.m_shared_data_writer_mock, update_block_rewards(block.m_hash, _, _)).WillOnce(Return(true));
	}

	auto result = m_component->pay_all(test_round_not_active_not_paid_data.m_round);
	EXPECT_TRUE(result);
	*/
}

