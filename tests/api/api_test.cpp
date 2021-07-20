#include <gtest/gtest.h>
#include <json/json.hpp>
#include "api_fixture.hpp"

std::vector< nlohmann::json> const requests_input
{
	R"({"jsonrpc": "2.0", "method": "get_meta_info", "id": 1})"_json,
	R"({"jsonrpc": "2.0", "method" : "get_latest_blocks", "id" : 2})"_json
};

std::vector<std::string> const invalid_requests_input
{
	"INVALID"
};

TEST_F(Api_fixture, simple_request_response) 
{

	network::Shared_payload received_payload{};

	m_api_server->start();

	network::Endpoint local_endpoint{ network::Transport_protocol::tcp, "127.0.0.1", 0 };
	auto socket = m_network_component->get_socket_factory()->create_socket(local_endpoint);

	auto connection = socket->connect(m_api_server->local_endpoint(), receive_handler(received_payload));
	EXPECT_TRUE(connection);

	wait_for_state(io_state::connected);

	// send requests
	for (auto& request : requests_input)
	{
		std::vector<std::uint8_t> request_buffer = nlohmann::json::to_bson(request);
		network::Shared_payload payload{ std::make_shared<network::Payload>(request_buffer.begin(), request_buffer.end()) };

		connection->transmit(payload);

		wait_for_state(io_state::receive);

		EXPECT_EQ(m_connect_handler_result, Result::Code::receive_ok);

		// compare request

		m_current_state = io_state::connected;
	}
}

TEST_F(Api_fixture, invalid_request)
{

	network::Shared_payload received_payload{};

	m_api_server->start();

	network::Endpoint local_endpoint{ network::Transport_protocol::tcp, "127.0.0.1", 0 };
	auto socket = m_network_component->get_socket_factory()->create_socket(local_endpoint);

	auto connection = socket->connect(m_api_server->local_endpoint(), receive_handler(received_payload));
	EXPECT_TRUE(connection);

	wait_for_state(io_state::connected);

	// send requests
	for (auto& request : invalid_requests_input)
	{
		network::Shared_payload payload{ std::make_shared<network::Payload>(request.begin(), request.end()) };

		connection->transmit(payload);

		m_io_context->poll();

		m_current_state = io_state::connected;
	}
}