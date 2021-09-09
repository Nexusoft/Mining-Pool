#ifndef NEXUSPOOL_REWARD_COMPONENT_HPP
#define NEXUSPOOL_REWARD_COMPONENT_HPP

#include "block.hpp"
#include <memory>

namespace nexuspool {
namespace reward {

enum class Difficulty_result : std::uint8_t { accept = 0, reject, block_found };

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    // Starts a new round
    virtual bool start_round() = 0;

    virtual bool is_round_active() = 0;

    // End round
    virtual bool end_round(std::uint32_t round_number) = 0;

    virtual Difficulty_result check_difficulty(const LLP::CBlock& block, std::uint32_t pool_nbits) const = 0;
    //pay all miners with unpaid shares.
    virtual void pay_all() const = 0;


};


}
}

#endif 
