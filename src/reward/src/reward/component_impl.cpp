#include "component_impl.hpp"
#include "common/utils.hpp"
#include <chrono>

namespace nexuspool {
namespace reward {

Component_impl::Component_impl(
	std::shared_ptr<spdlog::logger> logger, 
	nexus_http_interface::Component::Uptr http_interface,
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
	if (is_round_active())
	{
		m_payout_manager.calculate_reward_of_blocks(m_current_round);
	}
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

	round_data.m_total_rewards += m_payout_manager.calculate_reward_of_blocks(round_number);
	round_data.m_total_shares = m_data_reader->get_total_shares_from_accounts();

    if (round_data.m_total_rewards > 0 && round_data.m_total_shares > 0) // did the pool actually earned something this round?
    {
        // get all accounts which contribute to the current round
        auto const active_accounts = m_data_reader->get_active_accounts_from_round();
        for (auto& active_account : active_accounts)
        {
            // calculate reward for account. First reduce the total_rewards with pool_fee % 
            auto account_reward = (round_data.m_total_rewards * ( 1.0 - static_cast<double>(m_pool_fee / 100))) * (active_account.m_shares / round_data.m_total_shares);
            // add account to payment table (without datetime -> not paid yet)
            m_shared_data_writer->add_payment(persistance::Payment_data{ active_account.m_address, account_reward, active_account.m_shares, "", round_data.m_round });
        }
    }

    // reset shares of all accounts (round end)
    m_shared_data_writer->reset_shares_from_accounts();
	m_current_round = 0;

    // end round now
	round_data.m_is_active = false;
	m_shared_data_writer->update_round(round_data);

    return true;
}

Difficulty_result Component_impl::check_difficulty(const LLP::CBlock& block, uint32_t pool_nbits) const
{
	Difficulty_result result = Difficulty_result::reject;
	uint1024_t mainnet_difficulty_target;
	mainnet_difficulty_target.SetCompact(block.nBits);
	uint1024_t block_hash = block.GetHash();
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


	return result;

}

bool Component_impl::pay_all(std::uint32_t round)
{
	// check if round is ended and not paid and if pay_all is called after round_end_tme + grace period
	// to give last blocks a little time for possible mainnet confirmations
	auto round_data = m_data_reader->get_round(round);
	if (round_data.is_empty())
	{
		return false;
	}

	if (round_data.m_is_paid)
	{
		return false;	// already paid
	}

	if (round_data.m_is_active)
	{
		return false;	// round is still active
	}

	m_payout_manager.payout(m_account_from, m_pin, round);
	return true;
}

}
}
