#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include "LLP/packet.hpp"
#include "LLP/block.hpp"
#include "LLP/utils.hpp"

using namespace ::nexuspool;
using namespace ::testing;

Packet create_packet(std::uint8_t header, network::Payload data)
{
	Packet packet_input;
	packet_input.m_header = header;
	packet_input.m_data = std::make_shared<network::Payload>(data);
	packet_input.m_length = packet_input.m_data->size();
	packet_input.m_is_valid = true;

	return packet_input;
}

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

TEST(LLP_test, extract_block_height_packet_from_valid_data_buffer_test)
{
	std::uint32_t const height_input = 4873493U;
	Packet packet_input = create_packet(Packet::BLOCK_HEIGHT, uint2bytes(height_input));

 	network::Shared_payload valid_request = packet_input.get_bytes();
	std::size_t remaining_size = 0;
	auto packet = extract_packet_from_buffer(valid_request, remaining_size, 0);
	EXPECT_TRUE(packet.is_valid());
	EXPECT_EQ(bytes2uint(*packet.m_data), height_input);
	EXPECT_EQ(remaining_size, 0);
}

TEST(LLP_test, extract_block_data_packet_from_valid_data_buffer_test)
{
	LLP::CBlock const block;
	Packet packet_input = create_packet(Packet::BLOCK_DATA, block.serialize());

	network::Shared_payload valid_request = packet_input.get_bytes();
	std::size_t remaining_size = 0;
	auto packet = extract_packet_from_buffer(valid_request, remaining_size, 0);
	EXPECT_TRUE(packet.is_valid());
	EXPECT_EQ(remaining_size, 0);
}

TEST(LLP_test, extract_block_height_data_packet_from_valid_data_buffer_test)
{
	LLP::CBlock const block;
	std::uint32_t const height_input = 4873493U;
	Packet packet_height = create_packet(Packet::BLOCK_HEIGHT, uint2bytes(height_input));
	Packet packet_data = create_packet(Packet::BLOCK_DATA, block.serialize());

	network::Shared_payload valid_request = packet_height.get_bytes();
	auto const valid_request_original_size = valid_request->size();
	network::Shared_payload temp = packet_data.get_bytes();
	valid_request->insert(valid_request->end(), temp->begin(), temp->end());

	std::size_t remaining_size = 0;
	auto packet_1 = extract_packet_from_buffer(valid_request, remaining_size, 0);
	EXPECT_TRUE(packet_1.is_valid());
	EXPECT_EQ(bytes2uint(*packet_1.m_data), height_input);
	EXPECT_EQ(remaining_size, valid_request->size() - valid_request_original_size);

	// extract second paket
	auto packet_2 = extract_packet_from_buffer(valid_request, remaining_size, valid_request->size() - remaining_size);
	EXPECT_TRUE(packet_2.is_valid());
	EXPECT_EQ(remaining_size, 0);

}
