#ifndef NEXUSPOOL_REWARD_MANAGER_HPP
#define NEXUSPOOL_REWARD_MANAGER_HPP

#include <memory>
#include <cstdint>
#include "LLC/types/uint1024.h"

namespace nexuspool {
namespace reward {

class Manager {
public:
    using Uptr = std::unique_ptr<Manager>;

    virtual ~Manager() = default;

    // get the total weight for the current round.
    virtual std::uint64_t get_total_weight() const = 0;

    virtual std::uint64_t get_reward_of_miner(uint256_t session_key) const = 0;
    virtual std::uint64_t get_shares_of_miner(uint256_t session_key) const = 0;

    // Clear the Shares for the current round.
    virtual void clear_shares() = 0;

    virtual bool check_difficulty(double difficulty, double target_difficulty) const = 0;


};


}
}

#endif 
