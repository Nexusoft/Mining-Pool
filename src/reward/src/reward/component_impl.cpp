#include "component_impl.hpp"
#include "common/utils.hpp"
#include <chrono>

namespace nexuspool {
namespace reward {

Component_impl::Component_impl(
	std::shared_ptr<spdlog::logger> logger, 
	nexus_http_interface::Component::Uptr http_interface,
	persistance::Shared_data_writer::Sptr shared_data_writer, 
	persistance::Data_reader::Uptr data_reader)
    : m_logger{std::move(logger)}
	, m_http_interface{std::move(http_interface)}
    , m_shared_data_writer{ std::move(shared_data_writer) }
    , m_data_reader{ std::move(data_reader) }
	, m_current_round{0}
	, m_possible_found_blocks{}
{}

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
    auto const round_data = m_data_reader->get_latest_round();
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

    if (round_data.m_total_rewards > 0) // did the pool actually earned something this round?
    {
        // get all accounts which contribute to the current round
        auto const active_accounts = m_data_reader->get_active_accounts_from_round();
        for (auto& active_account : active_accounts)
        {
            // calculate reward for account
            auto account_reward = round_data.m_total_rewards * (active_account.m_shares / round_data.m_total_shares);
            // add account to payment table (without datetime -> not paid yet)
            m_shared_data_writer->add_payment(persistance::Payment_data{ active_account.m_address, account_reward, active_account.m_shares, "", round_data.m_round });
        }
    }

    // reset shares of all accounts (round end)
    m_shared_data_writer->reset_shares_from_accounts();
	m_current_round = 0;



    // end round now
   // round_data.m


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

void Component_impl::pay_all() const
{
	/*
	//lock the database.
	//Read from database to get sum of unpaid block rewards
	double total_block_rewards;
	//Read from the database to get a vector of addresses with unpaid shares and a corresponding vector of unpaid shares
	std::vector<std::string> addresses_to_be_paid;
	std::vector<double> shares_to_be_paid;
	//unlock database
	//Sum the total shares
	double total_shares;
	//calculate the value of one share
	double share_value = total_block_rewards / total_shares;
	auto total_recipients = addresses_to_be_paid.size();
	const auto max_payment_addresses = 99ull;
	auto recipients_to_be_paid = total_recipients;
	auto next_recipient_index = 0ull;
	while (recipients_to_be_paid > 0)
	{
		auto recipient_count_this_block = std::min(recipients_to_be_paid, max_payment_addresses);
		//make json string with addresses and amounts for up to 99 miners at a time.
		//post the api command to pay people.  the following is an example.
		cpr::Response r = cpr::Get(cpr::Url{ "https://api.github.com/repos/whoshuu/cpr/contributors" },
			cpr::Authentication{ "user", "pass" },
			cpr::Parameters{ {"anon", "true"}, {"key", "value"} });
		r.status_code;                  // 200
		r.header["content-type"];       // application/json; charset=utf-8
		r.text;                         // JSON text string
		//verify that it worked
		// update the database that the shares have been paid
		//do we need to wait 10 minutes between transactions to avoid fees?
		recipients_to_be_paid -= recipient_count_this_block;
	}

	*/


}

void Component_impl::add_block(std::string hash)
{
	m_possible_found_blocks.push_back(std::move(hash));
}

}
}
