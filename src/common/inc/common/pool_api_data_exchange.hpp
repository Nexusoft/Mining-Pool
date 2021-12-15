#ifndef NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_HPP
#define NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_HPP

#include <cstdint>
#include <memory>

namespace nexuspool {
namespace common {

class Pool_api_data_exchange
{
public:

	using Sptr = std::shared_ptr<Pool_api_data_exchange>;
	virtual ~Pool_api_data_exchange() = default;

	virtual std::uint32_t get_active_miners() const = 0;
	virtual void set_active_miners(std::uint32_t active_miners) = 0;
	virtual bool is_config_updated() const = 0;
	virtual void set_config_updated(bool update) = 0;
};

Pool_api_data_exchange::Sptr create_pool_api_data_exchange();

}
}
#endif