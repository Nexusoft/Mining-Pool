#ifndef NEXUSPOOL_REWARD_MANAGER_IMPL_HPP
#define NEXUSPOOL_REWARD_MANAGER_IMPL_HPP

#include "reward/manager.hpp"
#include <memory>

namespace nexuspool {
namespace reward {

class Manager_impl : public Manager 
{
public:

    Manager_impl();

    std::uint64_t get_total_shares() const override;

    std::uint64_t get_reward_of_miner(uint256_t session_key) const override;
    std::uint64_t get_shares_of_miner(uint256_t session_key) const override;

    void clear_shares() override;

    Difficulty_result check_difficulty(double difficulty, double target_difficulty) const override;

private:




};


}
}

#endif 
