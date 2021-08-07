#ifndef NEXUSPOOL_REWARD_COMPONENT_IMPL_HPP
#define NEXUSPOOL_REWARD_COMPONENT_IMPL_HPP

#include "reward/component.hpp"
#include "reward/manager_impl.hpp"

namespace nexuspool {
namespace reward {

class Component_impl : public Component
{
public:
    Component_impl(persistance::Shared_data_writer::Sptr shared_data_writer)
        : m_shared_data_writer{std::move(shared_data_writer)}
    {}

    Manager::Uptr create_reward_manager() override
    {
        return std::make_unique<Manager_impl>();
    }

private:

    persistance::Shared_data_writer::Sptr m_shared_data_writer;

};

}
}

#endif
