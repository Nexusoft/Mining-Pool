#include <gtest/gtest.h>
#include "nexus_http_interface_fixture.hpp"
#include "TAO/Ledger/difficulty.h"


using namespace ::nexuspool;

TEST(Nexus_http_interface, initialisation)
{
	auto logger = spdlog::stdout_color_mt("test_logger");
	auto nexus_http_interface_component = nexus_http_interface::create_component(logger, "", "", "");
	EXPECT_TRUE(nexus_http_interface_component);
}

TEST_F(Nexus_http_interface_fixture, get_block_hash_test)
{
	std::string blockhash{};
	auto result = m_component->get_block_hash(10, blockhash);
	EXPECT_TRUE(result);
	EXPECT_TRUE(!blockhash.empty());

}

TEST_F(Nexus_http_interface_fixture, get_mining_info_test)
{
	common::Mining_info mining_info;
	auto result = m_component->get_mining_info(mining_info);

	EXPECT_TRUE(result);
	EXPECT_GT(mining_info.m_height, 0);
	EXPECT_GT(mining_info.m_hash_rewards, 0);
	EXPECT_GT(mining_info.m_hash_difficulty, 0);
	EXPECT_GT(mining_info.m_prime_difficulty, 0);
	EXPECT_GT(mining_info.m_prime_reward, 0);
}

TEST_F(Nexus_http_interface_fixture, get_system_info_test)
{
	common::System_info system_info;
	auto result = m_component->get_system_info(system_info);

	EXPECT_TRUE(result);
	EXPECT_TRUE(system_info.is_valid());
}

TEST_F(Nexus_http_interface_fixture, get_block_reward_data_test)
{
	common::Block_reward_data reward_data{};
	std::string blockhash{};
	auto result = m_component->get_block_hash(10, blockhash);
	EXPECT_TRUE(result);

	result = m_component->get_block_reward_data(blockhash, reward_data);
	EXPECT_TRUE(result);
}

TEST_F(Nexus_http_interface_fixture, get_block_test)
{
	LLP::CBlock block;
	auto result = m_component->get_block(10, block);
	EXPECT_TRUE(result);

	//this is the actual difficulty of the block
	auto const block_hash = block.nChannel == 1 ? block.GetPrime() : block.GetHash();
	auto share_difficulty = TAO::Ledger::GetDifficulty(block_hash, block.nChannel);
}

TEST_F(Nexus_http_interface_fixture, does_account_exists_test)
{
	auto result = m_component->does_account_exists("testaddress");
	EXPECT_FALSE(result);

	result = m_component->does_account_exists("8BaWibxs1yLSSHLpeeVuEk7DdsSq6kTHp8pEkYoN27GR2HZSEcz");
	EXPECT_TRUE(result);
}

TEST_F(Nexus_http_interface_fixture, payout_test)
{
	std::string tx_id;
	nexus_http_interface::Payout_recipients payout_recipients{ {"asdasddas", 5}, {"zrztrtztrzre", 15}, {"yxcyxc", 50}, };
	m_component->payout("default", "1111", payout_recipients, tx_id);
}
