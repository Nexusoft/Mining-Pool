#ifndef NEXUSPOOL_REWARD_CREATE_COMPONENT_HPP
#define NEXUSPOOL_REWARD_CREATE_COMPONENT_HPP

#include "reward/component.hpp"
#include "persistance/data_writer.hpp"
#include <memory>

namespace nexuspool {
namespace reward {

// Component factory

Component::Uptr create_component(persistance::Shared_data_writer::Sptr shared_data_writer);

}
}

#endif 
