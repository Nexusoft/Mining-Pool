#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include "LLP/packet.hpp"

using namespace ::nexuspool;
using namespace ::testing;

TEST(LLP_test, basic_packet_test)
{
	Packet packet;
	EXPECT_FALSE(packet.is_valid());
}

TEST(LLP_test, extract_packet_from_invalid_buffer_test)
{
	network::Shared_payload invalid_buffer{};
	std::size_t remaining_size = 0;
	auto packet = extract_packet_from_buffer(invalid_buffer, remaining_size, 0);
	EXPECT_FALSE(packet.is_valid());
	EXPECT_EQ(remaining_size, 0);
}

TEST(LLP_test, extract_packet_from_empty_buffer_test)
{
	network::Shared_payload empty_buffer = std::make_shared<network::Payload>();
	std::size_t remaining_size = 0;
	auto packet = extract_packet_from_buffer(empty_buffer, remaining_size, 0);
	EXPECT_FALSE(packet.is_valid());
	EXPECT_EQ(remaining_size, 0);
}

TEST(LLP_test, extract_packet_from_incomplete_buffer_test)
{
	network::Shared_payload incomplete_buffer = std::make_shared<network::Payload>(network::Payload{1,2,3});
	std::size_t remaining_size = 0;
	auto packet = extract_packet_from_buffer(incomplete_buffer, remaining_size, 0);
	EXPECT_FALSE(packet.is_valid());
	EXPECT_EQ(remaining_size, 0);
}

TEST(LLP_test, extract_packet_from_buffer_with_wrong_start_index_test)
{
	network::Shared_payload incomplete_buffer = std::make_shared<network::Payload>(network::Payload{ 1,2,3 });
	std::size_t remaining_size = 0;
	auto packet = extract_packet_from_buffer(incomplete_buffer, remaining_size, incomplete_buffer->size());
	EXPECT_FALSE(packet.is_valid());
	EXPECT_EQ(remaining_size, 0);
}


TEST(LLP_test, extract_packet_from_valid_request_buffer_test)
{
	network::Shared_payload valid_request = std::make_shared<network::Payload>(network::Payload{ Packet::GET_HEIGHT });
	std::size_t remaining_size = 0;
	auto packet = extract_packet_from_buffer(valid_request, remaining_size, 0);
	EXPECT_TRUE(packet.is_valid());
	EXPECT_EQ(remaining_size, 0);
}
