#include <gtest/gtest.h>
#include "nexus_http_interface_fixture.hpp"


using namespace ::nexuspool;

TEST(Nexus_http_interface, initialisation)
{
	auto logger = spdlog::stdout_color_mt("test_logger");
	auto nexus_http_interface_component = nexus_http_interface::create_component(logger, "");
	EXPECT_TRUE(nexus_http_interface_component);
}

TEST_F(Nexus_http_interface_fixture, get_block_hash_test)
{
	std::string blockhash{};
	auto result = m_component->get_block_hash(10, blockhash);
	EXPECT_TRUE(result);
	EXPECT_TRUE(!blockhash.empty());

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

TEST_F(Nexus_http_interface_fixture, payout_test)
{
	nexus_http_interface::Payout_recipients payout_recipients{ {"asdasddas", 5}, {"zrztrtztrzre", 15}, {"yxcyxc", 50}, };
	m_component->payout("default", "1111", payout_recipients);
}
