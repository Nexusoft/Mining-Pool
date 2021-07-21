#include <gtest/gtest.h>
#include "persistance_fixture.hpp"


TEST(Persistance, initialisation)
{
	auto logger = spdlog::stdout_color_mt("test_logger");
	config::Persistance_config config{};
	auto component = persistance::create_component(logger, config);
	EXPECT_TRUE(component);

	component->start();
	component->stop();
}

TEST_F(Persistance_fixture, initialisation)
{

}