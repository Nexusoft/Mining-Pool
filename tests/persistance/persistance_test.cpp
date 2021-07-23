#include <gtest/gtest.h>
#include <sqlite/sqlite3.h>
#include "persistance_fixture.hpp"
#include "persistance/command/command.hpp"
#include <string>

using namespace persistance;
using ::nexuspool::persistance::command::Type;

TEST(Persistance, initialisation)
{
	auto logger = spdlog::stdout_color_mt("test_logger");
	config::Persistance_config config{};
	auto component = persistance::create_component(logger, config);
	EXPECT_TRUE(component);

	component->start();
	component->stop();
}

TEST_F(Persistance_fixture, command_factory_create_all_commands)
{
	std::vector<command::Type> command_types_input = {Type::create_db_schema, Type::get_banned_api_ip, Type::get_banned_user_and_ip};
	auto command_factory = m_persistance_component->get_command_factory();

	for (auto& command_type : command_types_input)
	{
		auto command = command_factory->create_command(command_type);
		EXPECT_EQ(command->get_type(), command_type);
	}
}

TEST_F(Persistance_fixture, simple_data_access)
{
	auto data_access = m_persistance_component->get_data_access_factory()->create_data_access();

	auto result = data_access->create_tables();
	EXPECT_TRUE(result);
}