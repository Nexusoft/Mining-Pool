#ifndef NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_HPP
#define NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include "common/types.hpp"

namespace nexuspool
{
namespace common 
{

class Pool_api_data_exchange
{
public:

	using Sptr = std::shared_ptr<Pool_api_data_exchange>;
	virtual ~Pool_api_data_exchange() = default;

	virtual std::uint32_t get_active_miners() const = 0;
	virtual void set_active_miners(std::uint32_t active_miners) = 0;
	virtual bool is_config_updated() const = 0;
	virtual void set_config_updated(bool update) = 0;
	virtual std::string get_payout_time() const = 0;
	virtual void set_payout_time(std::string payout_time) = 0;
	virtual std::uint32_t get_current_round() const = 0;
	virtual void set_current_round(std::uint32_t current_round) = 0;

	virtual Mining_info get_mining_info() const = 0;
	virtual void set_mining_info(Mining_info const& mining_info) = 0;
};

Pool_api_data_exchange::Sptr create_pool_api_data_exchange();

}
}
#endif