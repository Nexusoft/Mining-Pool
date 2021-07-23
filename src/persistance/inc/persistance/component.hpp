#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_HPP

#include "persistance/data_storage_factory.hpp"
#include "persistance/command/command_factory.hpp"
#include "persistance/data_access_factory.hpp"

namespace nexuspool {
namespace persistance {

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual Data_storage_factory::Sptr get_data_storage_factory() = 0;
    virtual command::Command_factory::Sptr get_command_factory() = 0;
    virtual Data_access_factory::Sptr get_data_access_factory() = 0;

};


}
}

#endif 
