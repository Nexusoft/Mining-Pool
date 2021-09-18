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

void Payout_manager::calculate_reward_of_blocks(std::vector<std::string>& blocks)
{
	for (auto& block : blocks)
	{
		common::Block_reward_data reward_data{};
		auto result = m_http_interface.get_block_reward_data(block, reward_data);
		if (!result)
		{
			m_logger->error("Couldn't receive block data from nxs wallet for hash {}", block);
			continue;
		}

		bool is_orphan = true;
		if (reward_data.m_tx_type == "COINBASE" && reward_data.m_tx_confirmations > 3)	// 3 confirmations enough?
		{
			is_orphan = false;
		}

		// update block in db
		if (!m_shared_data_writer.update_block_rewards(block, is_orphan, reward_data.m_reward))
		{
			m_logger->error("Couldn't update block in storage for hash {}", block);
		}
	}

	// clear given block vector because all data have been processed.
	blocks.clear();
}

void Payout_manager::payout(std::uint32_t current_round)
{

}

}
}
