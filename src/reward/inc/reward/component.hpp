#ifndef NEXUSPOOL_REWARD_COMPONENT_HPP
#define NEXUSPOOL_REWARD_COMPONENT_HPP

#include "manager.hpp"
#include <memory>

namespace nexuspool {
namespace reward {

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual Manager::Uptr create_reward_manager() = 0;

};


}
}

#endif 
