#ifndef TESTS_REWARD_REWARD_FIXTURE_HPP
#define TESTS_REWARD_REWARD_FIXTURE_HPP

#include <gtest/gtest.h>
#include "block.hpp"
#include <persistance/create_component.hpp>
#include <nexus_http_interface/create_component.hpp>
#include <reward/create_component.hpp>
#include <config/config.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cstdint>
#include <string>


namespace
{
using namespace ::nexuspool;
using ::nexuspool::reward::Component;

class Reward_fixture : public ::testing::Test
{
public:

	Reward_fixture()
	{
		m_logger = spdlog::stdout_color_mt("logger");
		m_logger->set_level(spdlog::level::debug);
		m_persistance_component = persistance::create_component(m_logger, m_config.get_persistance_config());

		m_component = reward::create_component(m_logger, 
			nexus_http_interface::create_component(m_logger, m_config.get_wallet_ip()), 
			m_persistance_component->get_data_writer_factory()->create_shared_data_writer(),
			m_persistance_component->get_data_reader_factory()->create_data_reader());
	}

protected:
	std::shared_ptr<spdlog::logger> m_logger;
	config::Config m_config;
	persistance::Component::Uptr m_persistance_component;
	Component::Uptr m_component;

	LLP::CBlock create_test_block()
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


	void TearDown() override
	{
		spdlog::drop("logger");
	}

};

}
#endif