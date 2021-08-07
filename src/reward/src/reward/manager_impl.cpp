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

}
}
