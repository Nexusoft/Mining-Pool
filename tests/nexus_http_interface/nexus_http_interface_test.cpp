#include <gtest/gtest.h>
#include "nexus_http_interface_fixture.hpp"


using namespace ::nexuspool;

TEST(Nexus_http_interface, initialisation)
{
	auto logger = spdlog::stdout_color_mt("test_logger");
	auto nexus_http_interface_component = nexus_http_interface::create_component(logger, "");
	EXPECT_TRUE(nexus_http_interface_component);
}

TEST_F(Nexus_http_interface_fixture, get_block_reward_data_test)
{
	common::Block_reward_data reward_data{};
	m_component->get_block_reward_data("8a1c90be7d0c6ab98342f0e4ee852dc2747a2f46e02c8d6448501a3ce613e08bc0485b572faf908948dff2fdcefafb1f553f9bd239053bffeca6934a7792804fcdc503ddd1bbe6af705ec1345c83f5752518849c2720a0630d6ecdbacca0d0f248ce405519249884a1fdd765d4b18beb7848620d077b3633c4588e72797f824e", reward_data);

}

TEST_F(Nexus_http_interface_fixture, payout_test)
{
	nexus_http_interface::Payout_recipients payout_recipients{ {"asdasddas", 5}, {"zrztrtztrzre", 15}, {"yxcyxc", 50}, };
	m_component->payout("default", "1111", payout_recipients);
}
