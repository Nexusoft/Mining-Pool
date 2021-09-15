#include <gtest/gtest.h>
#include "nexus_http_interface_fixture.hpp"


using namespace ::nexuspool;

TEST(Nexus_http_interface, initialisation)
{
	auto nexus_http_interface_component = nexus_http_interface::create_component();
	EXPECT_TRUE(nexus_http_interface_component);
}

TEST_F(Nexus_http_interface_fixture, connection_test)
{
	

}