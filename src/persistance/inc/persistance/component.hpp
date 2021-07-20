#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_HPP

#include "persistance/data_storage_factory.hpp"
#include "persistance/command/command_factory.hpp"
#include <memory>

namespace nexuspool {
namespace persistance {

class Component {
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual Data_storage_factory::Sptr get_data_storage_factory() = 0;
    virtual command::Command_factory::Sptr get_command_factory() = 0;
};


}
}

#endif 
