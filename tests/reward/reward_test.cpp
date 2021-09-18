#include <gtest/gtest.h>
#include "reward_fixture.hpp"

using namespace ::nexuspool;

TEST_F(Reward_fixture, create_component_with_active_round)
{
	EXPECT_CALL(*m_test_data.m_data_reader_mock, get_latest_round).WillOnce(Return(m_test_data.m_test_round_data));
	EXPECT_CALL(*m_test_data.m_data_reader_mock, get_blocks_from_round(m_test_data.m_test_current_round)).WillOnce(Return(m_test_data.m_test_blocks_from_round));


	m_component = reward::create_component(m_logger, std::make_unique<nexus_http_interface::Component_mock>(),
		m_persistance_component_mock->get_data_writer_factory()->create_shared_data_writer(),
		m_persistance_component_mock->get_data_reader_factory()->create_data_reader());

}

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