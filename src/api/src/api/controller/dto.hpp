#ifndef NEXUSPOOL_API_DTO_HPP
#define NEXUSPOOL_API_DTO_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace nexuspool
{
namespace api
{

#include OATPP_CODEGEN_BEGIN(DTO)

class Meta_infos_dto : public oatpp::DTO
{
	DTO_INIT(Meta_infos_dto, DTO)

	DTO_FIELD(Float64, pool_hashrate);
	DTO_FIELD(UInt16, round_duration);
	DTO_FIELD(UInt16, fee);
	DTO_FIELD(String, mining_mode);
	DTO_FIELD(UInt16, active_miners);
	DTO_FIELD(String, wallet_version);
	DTO_FIELD(String, pool_version);
	DTO_FIELD(String, payout_time);
	DTO_FIELD(UInt32, current_round);
};

class Account_dto : public oatpp::DTO
{
	DTO_INIT(Account_dto, DTO)

	DTO_FIELD(String, account);
	DTO_FIELD(String, created_at);
	DTO_FIELD(String, last_active);
	DTO_FIELD(UInt32, shares);
	DTO_FIELD(Float64, hashrate);
	DTO_FIELD(String, display_name);
};

class Payout_dto : public oatpp::DTO
{
	DTO_INIT(Payout_dto, DTO)

	DTO_FIELD(String, time);
	DTO_FIELD(Float64, amount);
	DTO_FIELD(Float64, shares);
	DTO_FIELD(String, txhash);

public:

	Payout_dto() = default;
	Payout_dto(const char* ptime, double pamount, double pshares, const char* ptxhash)
		: time(ptime)
		, amount(pamount)
		, shares(pshares)
		, txhash(ptxhash)
	{}
};

class Account_payouts_dto : public oatpp::DTO
{

	DTO_INIT(Account_payouts_dto, DTO)

	DTO_FIELD(Vector<Object<Payout_dto>>, payouts) = {};
};

class Block_dto : public oatpp::DTO
{
	DTO_INIT(Block_dto, DTO)

	DTO_FIELD(UInt32, round);
	DTO_FIELD(UInt32, height);
	DTO_FIELD(String, hash);
	DTO_FIELD(Float64, block_reward);
	DTO_FIELD(String, time);
	DTO_FIELD(Float64, network_diff);
	DTO_FIELD(Boolean, is_orphan);

public:

	Block_dto() = default;
	Block_dto(std::uint32_t pround, std::uint32_t pheight, const char* phash, double pblock_reward, const char* ptime, double pnetwork_diff, bool pis_orphan)
		: round(pround)
		, height(pheight)
		, hash(phash)
		, block_reward(pblock_reward)
		, time(ptime)
		, network_diff(pnetwork_diff)
		, is_orphan(pis_orphan)
	{}
};

class Latest_blocks_dto : public oatpp::DTO
{

	DTO_INIT(Latest_blocks_dto, DTO)

	DTO_FIELD(Vector<Object<Block_dto>>, blocks) = {};
};

class Hardware_dto : public oatpp::DTO
{
	DTO_INIT(Hardware_dto, DTO)

	DTO_FIELD(String, model);
	DTO_FIELD(Float64, hashrate);
	DTO_FIELD(Float64, power_consumption);

public:

	Hardware_dto() = default;
	Hardware_dto(const char* pmodel, double phashrate, double ppower_consumption)
		: model(pmodel)
		, hashrate(phashrate)
		, power_consumption(ppower_consumption)
	{}
};

class Hardware_data_dto : public oatpp::DTO
{
	DTO_INIT(Hardware_data_dto, DTO)

	DTO_FIELD(Vector<Object<Hardware_dto>>, devices) = {};
};

class Reward_data_dto : public oatpp::DTO
{
	DTO_INIT(Reward_data_dto, DTO)

	DTO_FIELD(Float64, block_reward);
	DTO_FIELD(Float64, network_diff);
};

class Statistics_longest_chain_dto : public oatpp::DTO
{
	DTO_INIT(Statistics_longest_chain_dto, DTO)

	DTO_FIELD(String, display_name);
	DTO_FIELD(Float64, chain);
};

class Statistics_block_finder_dto : public oatpp::DTO
{
	DTO_INIT(Statistics_block_finder_dto, DTO)

	DTO_FIELD(String, display_name);
	DTO_FIELD(UInt32, num_blocks);

public:

	Statistics_block_finder_dto() = default;
	Statistics_block_finder_dto(const char* pdisplay_name, std::uint32_t pnum_blocks)
		: display_name(pdisplay_name)
		, num_blocks(pnum_blocks)
	{}
};

class Statistics_top_block_finders_dto : public oatpp::DTO
{
	DTO_INIT(Statistics_top_block_finders_dto, DTO)

	DTO_FIELD(Vector<Object<Statistics_block_finder_dto>>, block_finders) = {};
};

#include OATPP_CODEGEN_END(DTO)

}
}

#endif