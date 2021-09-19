#include "payout_manager.hpp"
#include "common/utils.hpp"
#include <chrono>

namespace nexuspool {
namespace reward {

Payout_manager::Payout_manager(
	std::shared_ptr<spdlog::logger> logger,
	nexus_http_interface::Component& http_interface,
	persistance::Shared_data_writer& shared_data_writer,
	persistance::Data_reader& data_reader)
	: m_logger{ std::move(logger) }
	, m_http_interface{ http_interface}
	, m_shared_data_writer{ shared_data_writer }
	, m_data_reader{ data_reader }
{

}

void Payout_manager::calculate_reward_of_blocks(std::uint32_t round)
{
	auto blocks_update_count = 0U;
	m_logger->info("Loading blocks from current round {}", round);
	auto const blocks = m_data_reader.get_blocks_from_round(round);

	for (auto& block : blocks)
	{
		// all blocks which already have a reward for this round are filtered out
		if (block.m_mainnet_reward > 0)
		{
			continue;
		}

		common::Block_reward_data reward_data{};
		auto result = m_http_interface.get_block_reward_data(block.m_hash, reward_data);
		if (!result)
		{
			m_logger->error("Couldn't receive block data from nxs wallet for hash {}", block.m_hash);
			continue;
		}

		bool is_orphan = true;
		if (reward_data.m_tx_type == "COINBASE" && reward_data.m_tx_confirmations > 3)	// 3 confirmations enough?
		{
			is_orphan = false;
		}

		// update block in db
		if (!m_shared_data_writer.update_block_rewards(block.m_hash, is_orphan, reward_data.m_reward))
		{
			m_logger->error("Couldn't update block in storage for hash {}", block.m_hash);
		}
		++blocks_update_count;
	}
	m_logger->debug("Updated reward_data of {} blocks in round {}", blocks_update_count, round);
}

void Payout_manager::payout(std::uint32_t current_round)
{

}

}
}
