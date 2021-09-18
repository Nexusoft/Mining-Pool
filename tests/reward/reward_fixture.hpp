#ifndef TESTS_REWARD_REWARD_FIXTURE_HPP
#define TESTS_REWARD_REWARD_FIXTURE_HPP

#include <gtest/gtest.h>
#include "test_data.hpp"
#include "nexus_http_interface/component_mock.hpp"
#include "reward/create_component.hpp"
#include <config/config.hpp>
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
	config::Config m_config;

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
		m_component = reward::create_component(m_logger, std::make_unique<nexus_http_interface::Component_mock>(),
			m_persistance_component_mock->get_data_writer_factory()->create_shared_data_writer(),
			m_persistance_component_mock->get_data_reader_factory()->create_data_reader());
	}

protected:

};

}
#endif