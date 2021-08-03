#ifndef NEXUSPOOL_REWARD_COMPONENT_IMPL_HPP
#define NEXUSPOOL_REWARD_COMPONENT_IMPL_HPP

#include "reward/component.hpp"
#include "reward/manager_impl.hpp"

namespace nexuspool {
namespace reward {

class Component_impl : public Component
{
public:
    Component_impl() = default;

    Manager::Uptr create_reward_manager() override
    {
        return std::make_unique<Manager_impl>();
    }

private:

};

}
}

#endif
