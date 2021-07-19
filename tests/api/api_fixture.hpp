#ifndef TESTS_API_API_FIXTURE_HPP
#define TESTS_API_API_FIXTURE_HPP

#include <gtest/gtest.h>
#include <asio/io_context.hpp>
#include <config/config.hpp>
#include <network/create_component.hpp>
#include <persistance/create_component.hpp>
#include <api/server.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <thread>
#include <chrono>

namespace 
{
using namespace ::nexuspool;
using namespace ::nexuspool::network;

class Api_fixture : public ::testing::Test
{
public:

	Api_fixture()
		: m_config{}
		, m_io_context{ std::make_shared<::asio::io_context>() }
		, m_network_component{ network::create_component(m_io_context) }
		{
			m_logger = spdlog::stdout_color_mt("logger");
			m_logger->set_level(spdlog::level::debug);

			m_persistance_component = persistance::create_component(m_logger, m_config);
			auto data_storage = m_persistance_component->get_data_storage_factory()->create_data_storage();
			m_api_server = std::make_unique<api::Server>(m_logger, data_storage, "127.0.0.1", 0, m_network_component->get_socket_factory());
		}

protected:

	enum io_state { init = 0, connected, disconnect, transmit, receive };

	std::shared_ptr<spdlog::logger> m_logger;
	config::Config m_config;
	std::shared_ptr<::asio::io_context> m_io_context;
	std::unique_ptr<network::Component> m_network_component;
	std::unique_ptr<persistance::Component> m_persistance_component;
	std::unique_ptr<api::Server> m_api_server;
	io_state m_current_state{ io_state::init };
	Result::Code m_connect_handler_result{ Result::Code::error };

	void TearDown() override
	{
		spdlog::drop("logger");
	}

	void wait_for_state(const io_state& target_state, int timeout_ms = 5000)
	{
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

		while (m_current_state != target_state)
		{
			m_io_context->poll();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			ASSERT_TRUE(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() < timeout_ms);
		}

		m_io_context->reset();
	}

	Connection::Handler receive_handler(Shared_payload& received_payload)
	{
		return [this, &received_payload](auto result, auto received_buffer)
		{
			m_connect_handler_result = result;
			if (m_connect_handler_result == Result::Code::connection_ok)
			{
				m_current_state = io_state::connected;
			}
			else if (m_connect_handler_result == Result::Code::connection_closed)
			{
				m_current_state = io_state::disconnect;
			}
			else if (m_connect_handler_result == Result::Code::receive_ok)
			{
				received_payload = std::move(received_buffer);
				m_current_state = io_state::receive;
			}

		};
	}
};

}
#endif