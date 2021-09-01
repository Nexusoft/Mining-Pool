#include "manager_impl.hpp"
#include <spdlog/spdlog.h>


namespace nexuspool {
namespace reward {

Manager_impl::Manager_impl()
{}

std::uint64_t Manager_impl::get_total_shares() const
{
	std::uint64_t result;

	return result;

}

std::uint64_t Manager_impl::get_reward_of_miner(uint256_t session_key) const
{
	std::uint64_t result;

	return result;

}
std::uint64_t Manager_impl::get_shares_of_miner(uint256_t session_key) const
{
	std::uint64_t result;

	return result;

}

void Manager_impl::clear_shares()
{

}

Difficulty_result Manager_impl::check_difficulty(const LLP::CBlock& block, uint32_t pool_nbits) const
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

void Manager_impl::pay_all() const
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

}
}
