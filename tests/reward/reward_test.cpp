#include <gtest/gtest.h>
#include "reward_fixture.hpp"


using namespace ::nexuspool;

TEST(Reward, initialisation)
{
	auto reward_component = reward::create_component(nullptr, nullptr, nullptr);
	EXPECT_TRUE(reward_component);

	auto reward_manager = reward_component->create_reward_manager();
	EXPECT_TRUE(reward_manager);
}

TEST_F(Reward_fixture, difficulty_test)
{
	LLP::CBlock test_block = create_test_block();
	std::string test_block_hash = test_block.GetHash().ToString();

	std::uint32_t test_nbits = test_block.nBits;
	
	auto result = m_reward_manager->check_difficulty(test_block, test_nbits);
	EXPECT_EQ(result, nexuspool::reward::Difficulty_result::block_found);

	//change the test block a bit so it will be rejected by the pool
	test_block.nNonce++;
	test_block_hash = test_block.GetHash().ToString();
	result = m_reward_manager->check_difficulty(test_block, test_nbits);
	EXPECT_EQ(result, nexuspool::reward::Difficulty_result::reject);

	//todo: find example of a block that would be accpepted by the pool but not mainnet
	uint1024_t test_difficulty_threshold;
	test_difficulty_threshold.SetCompact(test_nbits);
	test_difficulty_threshold *= 2;
	std::uint32_t test_nbits_harder = test_difficulty_threshold.GetCompact();

	result = m_reward_manager->check_difficulty(test_block, test_nbits_harder);

}