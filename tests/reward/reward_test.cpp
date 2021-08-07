#include <gtest/gtest.h>
#include "reward/create_component.hpp"

using namespace ::nexuspool;

TEST(Reward, initialisation)
{
	auto reward_component = reward::create_component();
	EXPECT_TRUE(reward_component);

	auto reward_manager = reward_component->create_reward_manager();
	EXPECT_TRUE(reward_manager);
}

TEST(Reward, difficulty_test)
{
	auto reward_component = reward::create_component();
	auto reward_manager = reward_component->create_reward_manager();

	LLP::CBlock test_block;
	std::uint32_t test_nbits;
	auto result = reward_manager->check_difficulty(test_block, test_nbits);
}