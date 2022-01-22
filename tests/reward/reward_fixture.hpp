#ifndef TESTS_REWARD_REWARD_FIXTURE_HPP
#define TESTS_REWARD_REWARD_FIXTURE_HPP

#include <gtest/gtest.h>
#include "test_data.hpp"
#include "reward/create_component.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cstdint>
#include <string>
#include <memory>


namespace
{
using namespace ::nexuspool;
using ::nexuspool::reward::Component;

class Reward_fixture : public ::testing::Test
{
public:

	Reward_fixture()
	{
		m_logger = spdlog::stdout_color_mt("logger");
		m_logger->set_level(spdlog::level::debug);

		m_persistance_component_mock = m_test_data.create_persistance_component();
	}

protected:

	Test_data m_test_data;
	std::unique_ptr<persistance::Component_mock> m_persistance_component_mock;
	std::shared_ptr<spdlog::logger> m_logger;
	Component::Uptr m_component;

	void TearDown() override
	{
		spdlog::drop("logger");
	}

};

class Reward_fixture_created_component : public Reward_fixture
{
public:

	Reward_fixture_created_component()
	{
		m_component = reward::create_component(m_logger, m_test_data.m_timer_factory_mock, std::move(m_test_data.m_http_interface_mock),
			m_persistance_component_mock->get_data_writer_factory()->create_shared_data_writer(),
			m_persistance_component_mock->get_data_reader_factory()->create_data_reader(),
			"default", "1234", 1, 5);
	}

	void calculate_rewards_expectations(persistance::Round_data const& round_data, std::uint32_t round_number, std::vector<persistance::Block_data> const& blocks)
	{
		EXPECT_CALL(*m_test_data.m_data_reader_mock_raw, get_blocks_from_round(round_number)).WillOnce(Return(blocks));

		for (auto& block : blocks)
		{
			common::Block_reward_data reward_data{};
			EXPECT_CALL(*m_test_data.m_http_interface_mock_raw, get_block_reward_data(block.m_hash, _)).WillRepeatedly(Return(true));
			EXPECT_CALL(*m_test_data.m_shared_data_writer_mock, update_block_rewards(block.m_hash, _, _)).WillRepeatedly(Return(true));
		}
	}

protected:

};

}
#endif