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
};

class Account_dto : public oatpp::DTO
{

	DTO_INIT(Account_dto, DTO)

	DTO_FIELD(String, account);
	DTO_FIELD(String, created_at);
	DTO_FIELD(String, last_active);
	DTO_FIELD(UInt16, round_duration);
	DTO_FIELD(UInt32, shares);
	DTO_FIELD(UInt32, hashrate);
};

class Payout_dto : public oatpp::DTO
{
	DTO_INIT(Payout_dto, DTO)

	DTO_FIELD(String, time);
	DTO_FIELD(Float64, amount);
	DTO_FIELD(String, txhash);

public:

	Payout_dto(const char* ptime, double pamount, const char* ptxhash)
		: time(ptime)
		, amount(pamount)
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

		DTO_FIELD(UInt32, height);
	DTO_FIELD(String, hash);
	DTO_FIELD(Float64, block_reward);
	DTO_FIELD(String, time);
	DTO_FIELD(Float64, network_diff);

public:

	Block_dto(std::uint32_t pheight, const char* phash, double pblock_reward, const char* ptime, double pnetwork_diff)
		: height(pheight)
		, hash(phash)
		, block_reward(pblock_reward)
		, time(ptime)
		, network_diff(pnetwork_diff)
	{}
};

class Latest_blocks_dto : public oatpp::DTO
{

	DTO_INIT(Latest_blocks_dto, DTO)

		DTO_FIELD(Vector<Object<Block_dto>>, blocks) = {};
};

#include OATPP_CODEGEN_END(DTO)

}
}

#endif