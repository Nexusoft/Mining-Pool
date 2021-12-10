#ifndef NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_IMPL_HPP
#define NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_IMPL_HPP

#include "common/pool_api_data_exchange.hpp"
#include <mutex>

namespace nexuspool {
namespace common {

class Pool_api_data_exchange_impl : public Pool_api_data_exchange
{
public:

	std::uint32_t get_active_miners() const override;
	void set_active_miners(std::uint32_t active_miners) override;

private:

	std::uint32_t m_active_miners{ 0 };
	mutable std::mutex m_miners_mutex;
};

Pool_api_data_exchange::Sptr create_pool_api_data_exchange()
{
	return std::make_shared<Pool_api_data_exchange_impl>();
}


}
}

#endif
