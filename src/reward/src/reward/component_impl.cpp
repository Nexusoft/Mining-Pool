#include "component_impl.hpp"
#include "common/utils.hpp"
#include "TAO/Ledger/prime.h"
#include "TAO/Ledger/difficulty.h"
#include <chrono>
#include <cassert>

namespace nexuspool {
namespace reward {

Component_impl::Component_impl(
	std::shared_ptr<spdlog::logger> logger, 
	nexus_http_interface::Component::Sptr http_interface,
	persistance::Shared_data_writer::Sptr shared_data_writer, 
	persistance::Data_reader::Uptr data_reader,
	std::string account_from,
	std::string pin,
	std::uint16_t pool_fee)
    : m_logger{std::move(logger)}
	, m_http_interface{std::move(http_interface)}
    , m_shared_data_writer{ std::move(shared_data_writer) }
    , m_data_reader{ std::move(data_reader) }
	, m_current_round{0}
	, m_payout_manager{ m_logger, *m_http_interface, *m_shared_data_writer, *m_data_reader }
	, m_account_from{std::move(account_from)}
	, m_pin{std::move(pin)}
	, m_pool_fee{pool_fee}
{
}

Difficulty_result Component_impl::check_difficulty(const LLP::CBlock& block, uint32_t pool_nbits) const
{
	Difficulty_result result = Difficulty_result::reject;

	//hash channel
	if (block.nChannel == 2)
	{
		uint1024_t block_hash = block.GetHash();
		uint1024_t mainnet_difficulty_target;
		mainnet_difficulty_target.SetCompact(block.nBits);
		uint1024_t pool_difficulty_target;
		pool_difficulty_target.SetCompact(pool_nbits);
		if (block_hash < mainnet_difficulty_target)
		{
			result = Difficulty_result::block_found;
		}
		else if (block_hash < pool_difficulty_target)
		{
			result = Difficulty_result::accept;
		}
	}
	//prime channel
	else if (block.nChannel == 1)
	{
		uint1024_t block_prime = block.GetPrime();
		std::vector<uint8_t> offsets;
		double actual_prime_difficulty = TAO::Ledger::GetPrimeDifficulty(block_prime, offsets);
		double mainnet_difficulty_target = TAO::Ledger::GetDifficulty(block.nBits, block.nChannel);
		double pool_difficulty_target = TAO::Ledger::GetDifficulty(pool_nbits, block.nChannel);
		if (actual_prime_difficulty >= mainnet_difficulty_target)
		{
			result = Difficulty_result::block_found;
		}
		else if (actual_prime_difficulty >= pool_difficulty_target)
		{
			result = Difficulty_result::accept;
		}

	}
	else
	{
		result = Difficulty_result::reject;
	}

	return result;
}

bool Component_impl::start_round(std::uint16_t round_duration_hours)
{
	m_logger->info("Starting new round");
	// calc end_rount datetime
	auto end_round_time = std::chrono::system_clock::now();
	end_round_time += std::chrono::hours(round_duration_hours);
    if (!m_shared_data_writer->create_round(common::get_datetime_string(end_round_time)))
    {
        m_logger->error("Failed to create a new round!");
		return false;
    }
	auto const round_data = m_data_reader->get_latest_round();
	m_current_round = round_data.m_round;

	return true;
}

bool Component_impl::is_round_active()
{
	auto const round_data = m_data_reader->get_latest_round();
	if (round_data.m_is_active)
	{
		m_current_round = round_data.m_round;
		return true;
	}
	return false;
}

std::uint32_t Component_impl::get_current_round() const
{
	return m_current_round;
}

void Component_impl::get_start_end_round_times(std::chrono::system_clock::time_point& start_time, std::chrono::system_clock::time_point& end_time)
{
	auto const round_data = m_data_reader->get_latest_round();
	assert(!round_data.is_empty());

	start_time = common::get_timepoint_from_string(round_data.m_start_date_time, common::datetime_format);
	end_time = common::get_timepoint_from_string(round_data.m_end_date_time, common::datetime_format);

}

bool Component_impl::end_round(std::uint32_t round_number)
{
    auto round_data = m_data_reader->get_latest_round();
    if (round_data.m_round != round_number)
    {
        m_logger->error("Failed to end round {}. Latest round is {}", round_number, round_data.m_round);
        return false;
    }

    if (!round_data.m_is_active)
    {
        m_logger->error("Failed to end round {}. Round is already ended.", round_number);
        return false;
    }

	update_block_hashes(round_number);
	round_data.m_total_shares = m_data_reader->get_total_shares_from_accounts();
	if (round_data.m_total_shares > 0) // did the pool actually earned something this round?
	{
		// get all accounts which contribute to the current round
		auto const active_accounts = m_data_reader->get_active_accounts_from_round();
		for (auto const& active_account : active_accounts)
		{
			// add account to payment table (without datetime -> not paid yet)
			// reward is not set yet
			m_shared_data_writer->add_payment(persistance::Payment_data{ active_account.m_address, 0.0, active_account.m_shares, "", round_data.m_round, ""});
		}
	}

    // reset shares of all accounts (round end)
	if (!m_shared_data_writer->reset_shares_from_accounts())
	{
		m_logger->error("Failed to reset the shares from accounts in round {}", round_number);
	}
	m_current_round = 0;

    // end round now
	round_data.m_is_active = false;
	m_shared_data_writer->update_round(round_data);

    return true;
}

Calculate_rewards_result Component_impl::calculate_rewards(std::uint32_t round_number)
{
	Calculate_rewards_result result{ Calculate_rewards_result::failed};
	auto round_data = m_data_reader->get_round(round_number);
	if (round_data.is_empty())
	{
		m_logger->error("calculate_rewards error. Round {} does not exists.", round_number);
		return result;
	}
	if (round_data.m_is_paid)	// already paid
	{
		m_logger->error("calculate_rewards error. Round {} is already paid.", round_number);
		return result;
	}

	bool calculation_finished = false;
	round_data.m_total_rewards = m_payout_manager.calculate_reward_of_blocks(round_number, calculation_finished);
	if (calculation_finished)
	{
		if (round_data.m_total_rewards == 0.0)
		{
			// no blocks were found in this round -> no rewards
			m_logger->trace("Round {} calculate_reward_of_blocks is finished. No reward in this round", round_number);
			round_data.m_is_paid = true;
			m_shared_data_writer->update_round(round_data);
			result = Calculate_rewards_result::no_rewards;
			return result;
		}
		else
		{
			// now all account rewards can be credited
			auto payments = m_data_reader->get_not_paid_data_from_round(round_number);
			if (payments.empty())
			{
				return result;
			}
			for (auto& payment : payments)
			{
				// calculate reward for account. First reduce the total_rewards with pool_fee % 
				auto account_reward = (round_data.m_total_rewards * (1.0 - static_cast<double>(m_pool_fee / 100))) * (payment.m_shares / round_data.m_total_shares);
				m_shared_data_writer->update_reward_of_payment(account_reward, payment.m_account, round_number);
			}
			result = Calculate_rewards_result::finished;
			m_shared_data_writer->update_round(round_data);
			return result;
		}
	}
	else
	{
		m_logger->debug("Round {} calculate_reward_of_blocks is not finished yet.", round_number);
		return result;
	}
}

bool Component_impl::pay_round(std::uint32_t round)
{
	auto const calculate_rewards_result = calculate_rewards(round);
	if (calculate_rewards_result == Calculate_rewards_result::failed)
	{
		return false;
	}
	else if (calculate_rewards_result == Calculate_rewards_result::no_rewards)
	{
		return true;
	}

	// calculate_rewards is finished now
	// check if round is ended and not paid.
	auto round_data = m_data_reader->get_round(round);
	if (round_data.m_is_active)
	{
		m_logger->error("pay_round error. Round {} is still active.", round);
		return false;	// round is still active
	}
	if (round_data.m_is_paid)
	{
		m_logger->error("pay_round error. Round {} is already paid.", round);
		return false;	// round is already paid
	}

	if (m_payout_manager.payout(m_account_from, m_pin, round))
	{
		round_data.m_is_paid = true;
		m_shared_data_writer->update_round(round_data);
	}

	// cleanup empty payment records of this round
	m_shared_data_writer->delete_empty_payments();

	return true;
}

void Component_impl::update_block_hashes_from_current_round()
{
	assert(m_current_round != 0);
	update_block_hashes(m_current_round);
}

void Component_impl::update_block_hashes(std::uint32_t round)
{
	auto updated_blocks = 0U;
	auto const block_heights = m_data_reader->get_blocks_without_hash_from_round(round);
	for (auto& height : block_heights)
	{
		std::string block_hash{};
		auto result = m_http_interface->get_block_hash(height, block_hash);
		if (!result)
		{
			continue;
		}

		result = m_shared_data_writer->update_block_hash(height, block_hash);
		if (result)
		{
			updated_blocks++;
		}
		else
		{
			m_logger->error("Failed to update block_hash for height {}", height);
		}
	}

	if (updated_blocks > 0)
	{
		m_logger->debug("Updated block hashes from {} blocks in round {}", updated_blocks, round);
	}
}

bool Component_impl::process_unpaid_rounds()
{
	auto const round_numbers = m_data_reader->get_unpaid_rounds();
	if (round_numbers.empty())
	{
		return true;
	}

	for (auto& round_number : round_numbers)
	{
		update_block_hashes(round_number);
		auto result = pay_round(round_number);
		if (!result)
		{
			m_logger->error("process_unpaid_rounds: pay_round of round {} failed.", round_number);
		}
	}
	return true;
}

void Component_impl::block_found()
{
	auto round_data = m_data_reader->get_latest_round();
	round_data.m_blocks++;
	m_shared_data_writer->update_round(round_data);
}

}
}
