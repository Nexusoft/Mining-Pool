#include <gtest/gtest.h>
#include "reward/manager.hpp"

TEST(Reward, initialisation)
{
	nexuspool::reward::Manager::Uptr reward_manager{};
	EXPECT_FALSE(reward_manager);
}