#ifndef NEXUSPOOL_REWARD_COMPONENT_HPP
#define NEXUSPOOL_REWARD_COMPONENT_HPP

#include "LLP/block.hpp"
#include <string>
#include <memory>
#include <chrono>

namespace nexuspool {
namespace reward {

enum class Difficulty_result : std::uint8_t { accept = 0, reject, block_found };
enum class Calculate_rewards_result : std::uint8_t { finished = 0, failed, no_rewards };

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual Difficulty_result check_difficulty(const LLP::CBlock& block, std::uint32_t pool_nbits) const = 0;

    // Starts a new round
    virtual bool start_round(std::uint16_t round_duration_hours) = 0;

    // Check if the current round is active or not
    virtual bool is_round_active() = 0;
    virtual std::uint32_t get_current_round() const = 0;
    virtual void get_start_end_round_times(std::chrono::system_clock::time_point& start_time, std::chrono::system_clock::time_point& end_time) = 0;

    // End round
    virtual bool end_round(std::uint32_t round_number) = 0;

    // calculate the rewards for each miner for the given round
    virtual Calculate_rewards_result calculate_rewards(std::uint32_t round_number) = 0;
    //pay all miners with unpaid shares.
    virtual bool pay_round(std::uint32_t round) = 0;

    // check all unpaid rounds (update block rewards, calculate round rewards etc)
    virtual bool process_unpaid_rounds() = 0;

    // increments the block count for the current round in storage
    virtual void block_found() = 0;

    // Updates the block hashes from all blocks in the current active round
    virtual void update_block_hashes_from_current_round() = 0;


};


}
}

#endif 
