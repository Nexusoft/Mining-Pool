#include <gtest/gtest.h>
#include <json/json.hpp>
#include "api_fixture.hpp"

std::vector< nlohmann::json> const requests_input
{
	R"({"jsonrpc": "2.0", "method": "get_meta_info", "id": 1})"_json,
	R"({"jsonrpc": "2.0", "method" : "get_latest_blocks", "id" : 2})"_json,
	R"(	{"jsonrpc": "2.0", "method" : "get_account", "params" : ["_account"], "id" : 3})"_json,
	R"({"jsonrpc": "2.0", "method": "get_account_header", "params": ["_account"], "id": 4})"_json,
	R"({"jsonrpc": "2.0", "method": "get_account_works", "params": ["_account"], "id": 5})"_json,
	R"({"jsonrpc": "2.0", "method": "get_account_payouts", "params": ["_account"], "id": 6})"_json
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

	auto response_id = 1U;
	// send requests
	for (auto& request : requests_input)
	{
		std::vector<std::uint8_t> request_buffer = nlohmann::json::to_bson(request);
		network::Shared_payload payload{ std::make_shared<network::Payload>(request_buffer.begin(), request_buffer.end()) };

		connection->transmit(payload);

		wait_for_state(io_state::receive);

		EXPECT_EQ(m_connect_handler_result, Result::Code::receive_ok);

		// compare request
		auto result_json = nlohmann::json::from_bson(*received_payload);
		EXPECT_EQ(result_json.at("jsonrpc"), "2.0");
		EXPECT_TRUE(result_json.count("id") != 0);
		EXPECT_EQ(result_json.at("id"), response_id);
		//EXPECT_TRUE(result_json.count("result") != 0);

		response_id++;
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

		EXPECT_FALSE(received_payload);	// no answer from server
		m_current_state = io_state::connected;
	}
}

TEST_F(Api_fixture, get_account_with_invalid_accountname)
{

	network::Shared_payload received_payload{};

	m_api_server->start();

	network::Endpoint local_endpoint{ network::Transport_protocol::tcp, "127.0.0.1", 0 };
	auto socket = m_network_component->get_socket_factory()->create_socket(local_endpoint);

	auto connection = socket->connect(m_api_server->local_endpoint(), receive_handler(received_payload));
	EXPECT_TRUE(connection);

	wait_for_state(io_state::connected);

	// send request get_account with invalid accountname
	auto json_request = R"(	{"jsonrpc": "2.0", "method" : "get_account", "params" : ["_account"], "id" : 3})"_json;
	std::vector<std::uint8_t> request_buffer = nlohmann::json::to_bson(json_request);
	network::Shared_payload payload{ std::make_shared<network::Payload>(request_buffer.begin(), request_buffer.end()) };

	connection->transmit(payload);

	wait_for_state(io_state::receive);

	EXPECT_EQ(m_connect_handler_result, Result::Code::receive_ok);

	// compare request
	auto result_json = nlohmann::json::from_bson(*received_payload);
	EXPECT_EQ(result_json.at("jsonrpc"), "2.0");
	EXPECT_TRUE(result_json.count("id") != 0);
	EXPECT_TRUE(result_json.count("error") != 0);

	auto json_error = result_json["error"];
	EXPECT_TRUE(json_error.count("code") != 0);
	EXPECT_TRUE(json_error.count("message") != 0);


}

