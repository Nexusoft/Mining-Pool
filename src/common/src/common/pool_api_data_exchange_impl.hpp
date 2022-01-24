#ifndef NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_IMPL_HPP
#define NEXUSPOOL_COMMON_POOL_API_DATA_EXCHANGE_IMPL_HPP

#include "common/pool_api_data_exchange.hpp"
#include <mutex>
#include <atomic>
#include <string>

namespace nexuspool {
namespace common {

class Pool_api_data_exchange_impl : public Pool_api_data_exchange
{
public:

	std::uint32_t get_active_miners() const override;
	void set_active_miners(std::uint32_t active_miners) override;
	bool is_config_updated() const override;
	void set_config_updated(bool update) override;
	std::string get_payout_time() const override;
	void set_payout_time(std::string payout_time) override;
	Mining_info get_mining_info() const override;
	void set_mining_info(Mining_info const& mining_info) override;

private:

	std::atomic_uint32_t m_active_miners{ 0 };
	Mining_info m_mining_info{};
	std::atomic_bool m_config_updated{ true };
	std::string m_payout_time{};
	mutable std::mutex m_payout_time_mutex;
	mutable std::mutex m_mining_info_mutex;
};

Pool_api_data_exchange::Sptr create_pool_api_data_exchange()
{
	return std::make_shared<Pool_api_data_exchange_impl>();
}


}
}

#endif
