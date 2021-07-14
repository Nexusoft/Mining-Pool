#ifndef TESTS_API_API_FIXTURE_HPP
#define TESTS_API_API_FIXTURE_HPP

#include <gtest/gtest.h>
#include <asio/io_context.hpp>
#include <config/config.hpp>
#include <network/create_component.hpp>
#include <persistance/create_component.hpp>
#include <api/server.hpp>

namespace 
{
using namespace ::nexuspool;

class Api_fixture : public ::testing::Test
{
public:

	Api_fixture()
		: m_config{}
		, m_io_context{ std::make_shared<::asio::io_context>() }
		, m_network_component{ network::create_component(m_io_context) }
		, m_persistance_component{ persistance::create_component(m_config) }
		{
			auto data_storage = m_persistance_component->get_data_storage_factory()->create_data_storage();
			m_api_server = std::make_unique<api::Server>(data_storage, "127.0.0.1", 0, m_network_component->get_socket_factory());
		}

	config::Config m_config;
	std::shared_ptr<::asio::io_context> m_io_context;
	std::unique_ptr<network::Component> m_network_component;
	std::unique_ptr<persistance::Component> m_persistance_component;
	std::unique_ptr<api::Server> m_api_server;
};

}
#endif