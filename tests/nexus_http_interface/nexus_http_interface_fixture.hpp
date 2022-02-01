#ifndef TESTS_NEXUS_HTTP_INTERFACE_NEXUS_HTTP_INTERFACE_FIXTURE_HPP
#define TESTS_NEXUS_HTTP_INTERFACE_NEXUS_HTTP_INTERFACE_FIXTURE_HPP

#include <gtest/gtest.h>
#include <nexus_http_interface/create_component.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cstdint>
#include <string>


namespace
{
using namespace ::nexuspool;
using ::nexuspool::nexus_http_interface::Component;

class Nexus_http_interface_fixture : public ::testing::Test
{
public:

	Nexus_http_interface_fixture()
	{
		m_logger = spdlog::stdout_color_mt("logger");
		m_logger->set_level(spdlog::level::debug);

		m_component = nexus_http_interface::create_component(m_logger, "127.0.0.1", "test", "1234");
	}

protected:
	std::shared_ptr<spdlog::logger> m_logger;
	Component::Sptr m_component;

	void TearDown() override
	{
		spdlog::drop("logger");
	}

};

}
#endif