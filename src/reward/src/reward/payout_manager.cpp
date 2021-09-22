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

double Payout_manager::calculate_reward_of_blocks(std::uint32_t round)
{
	double total_rewards = 0;
	auto blocks_update_count = 0U;
	auto blocks_orphaned = 0U;
	m_logger->info("Loading blocks from round {}", round);
	auto const blocks = m_data_reader.get_blocks_from_round(round);

	for (auto& block : blocks)
	{
		// all blocks which already have a reward for this round are filtered out
		if (block.m_mainnet_reward > 0)
		{
			total_rewards += block.m_mainnet_reward;
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
		else
		{
			++blocks_orphaned;
		}

		// update block in db
		if (!m_shared_data_writer.update_block_rewards(block.m_hash, is_orphan, reward_data.m_reward))
		{
			m_logger->error("Couldn't update block in storage for hash {}", block.m_hash);
		}
		if (!is_orphan)
		{
			total_rewards += reward_data.m_reward;
		}

		++blocks_update_count;
	}
	m_logger->debug("Updated reward_data of {} blocks in round {}. {} blocks are ORPHAN. Total rewards calculated {}", 
		blocks_update_count, round, blocks_orphaned, total_rewards);

	return total_rewards;
}

bool Payout_manager::payout(std::string const& account_from, std::string const& pin, std::uint32_t current_round)
{
	nexus_http_interface::Payout_recipients payout_recipients{};
	auto const accounts = m_data_reader.get_active_accounts_from_round();

	// update total_shares of round

	auto result = m_http_interface.payout(account_from, pin, payout_recipients);
	if (!result)
	{
		m_logger->error("Couldn't pay miners. {} accounts from round {} not paid!", accounts.size(), current_round);
		return false;
	}
	return true;
}

}
}
