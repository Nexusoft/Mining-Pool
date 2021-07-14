#include <gtest/gtest.h>
#include <json/json.hpp>
#include "api_fixture.hpp"


TEST_F(Api_fixture, simple_request_response) 
{
	nlohmann::json test_request(R"({"jsonrpc": "2.0", "method": "test", "params": [50, 2], "id": 1})");
	std::string test_request_string{ test_request.dump() };
	network::Shared_payload payload{ std::make_shared<network::Payload>(test_request_string.begin(), test_request_string.end()) };
	network::Shared_payload received_payload{};

	m_api_server->start();

	network::Endpoint local_endpoint{ network::Transport_protocol::tcp, "127.0.0.1", 0 };
	auto socket = m_network_component->get_socket_factory()->create_socket(local_endpoint);

	auto connection = socket->connect(m_api_server->local_endpoint(), receive_handler(received_payload));
	EXPECT_TRUE(connection);

	wait_for_state(io_state::connected);

	connection->transmit(payload);

	wait_for_state(io_state::receive);

	EXPECT_EQ(m_connect_handler_result, Result::Code::receive_ok);
}