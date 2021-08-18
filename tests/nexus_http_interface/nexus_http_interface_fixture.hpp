#ifndef TESTS_NEXUS_HTTP_INTERFACE_NEXUS_HTTP_INTERFACE_FIXTURE_HPP
#define TESTS_NEXUS_HTTP_INTERFACE_NEXUS_HTTP_INTERFACE_FIXTURE_HPP

#include <gtest/gtest.h>
#include <nexus_http_interface/create_component.hpp>
#include <config/config.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cstdint>
#include <string>


namespace
{
using namespace ::nexuspool;
using ::nexuspool::nexus_http_interface::Component;
using ::nexuspool::nexus_http_interface::Manager;

class Nexus_http_interface_fixture : public ::testing::Test
{
public:

	Nexus_http_interface_fixture()
	{
		m_logger = spdlog::stdout_color_mt("logger");
		m_logger->set_level(spdlog::level::debug);

		m_component = nexus_http_interface::create_component();
		m_nexus_http_interface_manager = m_component->create_nexus_http_interface_manager();
	}

protected:
	std::shared_ptr<spdlog::logger> m_logger;
	config::Config m_config;
	Component::Uptr m_component;
	Manager::Uptr m_nexus_http_interface_manager;

	


	void TearDown() override
	{
		spdlog::drop("logger");
	}

};

}
#endif