#include <gtest/gtest.h>
#include "api_fixture.hpp"


TEST_F(Api_fixture, simple_request_response) 
{
	network::Endpoint local_endpoint{ network::Transport_protocol::tcp, "127.0.0.1", 0 };
	auto socket = m_network_component->get_socket_factory()->create_socket(local_endpoint);

	auto connection = socket->connect(m_api_server->local_endpoint(), [](auto, auto) {});
	EXPECT_TRUE(connection);
}