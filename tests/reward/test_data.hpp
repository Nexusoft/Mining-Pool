#ifndef TESTS_REWARD_TEST_DATA_HPP
#define TESTS_REWARD_TEST_DATA_HPP

#include <string>
#include <vector>
#include <cstdio>
#include <gtest/gtest.h>
#include "LLP/block.hpp"
#include "persistance/component_mock.hpp"
#include "persistance/data_reader_factory_mock.hpp"
#include "persistance/data_writer_factory_mock.hpp"
#include "persistance/data_reader_mock.hpp"
#include "persistance/data_writer_mock.hpp"
#include "nexus_http_interface/component_mock.hpp"
#include "chrono/timer_factory_mock.hpp"

namespace
{
using namespace ::nexuspool;
using namespace ::testing;

std::uint32_t const test_current_round{ 3 };
std::uint32_t const test_unpaid_round{ 4 };
persistance::Round_data const test_round_data{ test_current_round, 0, 0, 0, "2021-09-19 10:20:04.0000000", "2021-09-20 10:20:04.0000000", true, false };
std::vector<persistance::Block_data> const test_blocks_from_round{
	{ "testblockhash1", 50001, "hash", 351.64, false, "", test_current_round, "2021-09-19 13:36:14.0000000", 2.546},
	{ "testblockhash2", 50002, "hash", 352.64, false, "", test_current_round, "2021-09-19 18:50:45.0000000", 2.546},
	{ "testblockhash3", 50003, "prime", 8.64, false, "", test_current_round, "2021-09-19 19:00:59.0000000", 2.546},
};

std::vector<persistance::Account_data_for_payment> const test_active_accounts_from_round{
	{"accountaddress1", 10},
	{"accountaddress2", 5},
	{"accountaddress3", 50},
	{"accountaddress4", 9},
	{"accountaddress5", 1}
};


persistance::Round_data const test_round_not_active_not_paid_data{ test_unpaid_round, 0, 0, 0, "2021-09-19 10:20:04.0000000", "2021-09-20 10:20:04.0000000", false, false };
persistance::Round_data const test_round_not_active_paid_data{ test_current_round, 20, 5, 2, "2021-09-20 12:00:44.0000000", "2021-09-21 12:00:44.0000000", false, true };
std::vector<persistance::Block_data> const test_blocks_from_unpaid_round{
	{ "testblockhash1", 60001, "hash", 351.64, false, "", test_unpaid_round, "2021-09-19 13:36:14.0000000", 0},
	{ "testblockhash2", 60002, "hash", 352.64, false, "", test_unpaid_round, "2021-09-19 18:50:45.0000000", 0},
	{ "testblockhash3", 60003, "hash", 372.64, false, "", test_unpaid_round, "2021-09-19 19:00:59.0000000", 0},
	{ "testblockhash4", 60004, "hash", 382.64, false, "", test_unpaid_round, "2021-09-19 19:00:59.0000000", 0},
	{ "testblockhash5", 60005, "hash", 399.64, false, "", test_unpaid_round, "2021-09-19 19:00:59.0000000", 0},
};

class Test_data
{
public:

	Test_data()
	{
		m_persistance_component_mock = std::make_unique<NiceMock<persistance::Component_mock>>();
		m_data_reader_factory_mock = std::make_shared<NiceMock<persistance::Data_reader_factory_mock>>();
		m_data_writer_factory_mock = std::make_shared<NiceMock<persistance::Data_writer_factory_mock>>();
		m_data_reader_mock = std::make_unique<NiceMock<persistance::Data_reader_mock>>();
		m_shared_data_writer_mock = std::make_shared<NiceMock<persistance::Shared_data_writer_mock>>();
		m_data_reader_mock_raw = m_data_reader_mock.get();

		m_http_interface_mock = std::make_unique<nexus_http_interface::Component_mock>();
		m_http_interface_mock_raw = m_http_interface_mock.get();
		m_timer_factory_mock = std::make_shared<NiceMock<chrono::Timer_factory_mock>>();
	}

	~Test_data()
	{

	}

	LLP::CBlock create_hash_channel_test_block()
	{
		LLP::CBlock test_block;

		// use a sample Nexus block as a test vector
		std::uint32_t nHeight = 2023276;
		std::uint32_t nVersion = 4;
		std::uint32_t nChannel = 2;
		std::uint32_t nBits = 0x7b032ed8;
		std::uint64_t nNonce = 21155560019;
		std::string merkleStr = "31f5a458fc4207cd30fd1c4f43c26a3140193ed088f75004aa5b07beebf6be905fd49a412294c73850b422437c414429a6160df514b8ec919ea8a2346d3a3025";
		std::string hashPrevBlockStr = "00000902546301d2a29b00cad593cf05c798469b0e3f39fe623e6762111d6f9eed3a6a18e0e5453e81da8d0db5e89808e68e96c8df13005b714b1e63d7fa44a5025d1370f6f255af2d5121c4f65624489f1b401f651b5bd505002d3a5efc098aa6fa762d270433a51697d7d8d3252d56bbbfbe62f487f258c757690d31e493a7";
		std::string expectedHashResultStr = "00000000000072d507b3b1cf8829e6e8201cd5288494b53b379e9f33fcaeeec82d1415330bbb4746354db60b3dbb86ed5008d27877ada92194e5d54d3bfb247ede1b0305db3f208e7e4a51a237dcb0ccc342d345ad7221f0bbe30561e517d0adb2190870bd24ab6b17e9dd895383f183eab21d5d045e438ad9c3d004983eed6b";
		std::uint64_t expectedHashUpper64Bits = 0x72d5;

		test_block.nHeight = nHeight;
		test_block.nVersion = nVersion;
		test_block.nChannel = nChannel;
		test_block.nBits = nBits;
		test_block.nNonce = nNonce;
		test_block.hashMerkleRoot.SetHex(merkleStr);
		test_block.hashPrevBlock.SetHex(hashPrevBlockStr);

		return test_block;
	}

	LLP::CBlock create_prime_channel_test_block()
	{	
		LLP::CBlock test_block;

		// use a sample Nexus block as a test vector
		std::uint32_t nHeight = 2023281;
		std::uint32_t nVersion = 4;
		std::uint32_t nChannel = 1;
		std::uint32_t nBits = 0x04edbf41;
		std::uint64_t nNonce = 4855405147206064663;
		std::string merkleStr = "753608706d2e1a660e8bb781cfafb1bd10b183a9cfb614b6999687d029caeb5dd5553f58c9d9dfc45fd6254ebb5bd6479ce9fde428177a04096ea67120478003";
		std::string hashPrevBlockStr = "00000000000143e9a77aa469d90d2da3a2cae76de6f978af11cf4094571cf1c7c2410759ca0f7d373d74703b0e822c18413bc0e7d506b83afdcce40b685c3da306646eea9785c034fe53efb66761c249801d456340027320813afa7ba6dbe4f9cd711aa18ec2cc1c514598fca2ccad9630c0e7c19901da41d0cbfcf5c53665a2";
		std::string expectedHashResultStr = "000008b60c656453f28d18ed2fd27745e9468b1cd4269366b81755b1266e872bdf5623ec40aa40d491319f511cb9cc6a9884177a5f7228c3ff0c29a24d9f4e8b6dc48d4765107f8f5cd32494096823a8f53f5d1ef6b17e4b9c2e9aed620bf8415dabd93ff613730fac3677198545ea99c6bfc780fd15c8e21b98af0db461bf8a";
		std::uint64_t expectedHashUpper64Bits = 0x000008b60c656453;
		double expected_difficulty = 8.2689857;

		test_block.nHeight = nHeight;
		test_block.nVersion = nVersion;
		test_block.nChannel = nChannel;
		test_block.nBits = nBits;
		test_block.nNonce = nNonce;
		test_block.hashMerkleRoot.SetHex(merkleStr);
		test_block.hashPrevBlock.SetHex(hashPrevBlockStr);

		return test_block;
	}

	std::unique_ptr<persistance::Component_mock> create_persistance_component()
	{
		ON_CALL(*m_persistance_component_mock, get_data_writer_factory()).WillByDefault(Return(m_data_writer_factory_mock));
		ON_CALL(*m_persistance_component_mock, get_data_reader_factory()).WillByDefault(Return(m_data_reader_factory_mock));

		ON_CALL(*m_data_writer_factory_mock, create_shared_data_writer_impl()).WillByDefault(Return(m_shared_data_writer_mock));
		ON_CALL(*m_data_reader_factory_mock, create_data_reader_impl()).WillByDefault(Return(ByMove(std::move(m_data_reader_mock))));

		return std::move(m_persistance_component_mock);
	}


	std::shared_ptr<persistance::Data_reader_factory_mock> m_data_reader_factory_mock;
	std::shared_ptr<persistance::Data_writer_factory_mock> m_data_writer_factory_mock;
	std::unique_ptr<persistance::Data_reader_mock> m_data_reader_mock;
	persistance::Data_reader_mock* m_data_reader_mock_raw{ nullptr };
	std::shared_ptr<persistance::Shared_data_writer_mock> m_shared_data_writer_mock;
	std::unique_ptr<nexus_http_interface::Component_mock> m_http_interface_mock;
	nexus_http_interface::Component_mock* m_http_interface_mock_raw{ nullptr };
	std::shared_ptr<chrono::Timer_factory_mock> m_timer_factory_mock;

protected:

	std::unique_ptr<persistance::Component_mock> m_persistance_component_mock;




};

}
#endif