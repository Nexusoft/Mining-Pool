#include <gtest/gtest.h>
#include <sqlite/sqlite3.h>
#include "persistance_fixture.hpp"
#include "persistance/command/command.hpp"
#include <string>

using namespace persistance;

TEST(Persistance, initialisation)
{
	auto logger = spdlog::stdout_color_mt("test_logger");
	config::Persistance_config config{};
	auto component = persistance::create_component(logger, config);
	EXPECT_TRUE(component);

	component->start();
	component->stop();
}

TEST_F(Persistance_fixture, command_factory)
{
	auto command_factory = m_persistance_component->get_command_factory();
	auto command = 
		command_factory->create_command(command::Type::create_db_schema);

}