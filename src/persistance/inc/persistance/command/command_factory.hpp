#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_HPP

#include "persistance/command/types.hpp"
#include "persistance/command/command.hpp"
#include <memory>
#include <any>

namespace nexuspool {
namespace persistance {
namespace command {

class Command_factory 
{
public:

    using Sptr = std::shared_ptr<Command_factory>;

    Command::Sptr create_command(Type command_type);

private:

    virtual Command::Sptr create_command_impl(Type command_type) = 0;
};

inline Command::Sptr Command_factory::create_command(Type command_type)
{
    // implements
    return create_command_impl(command_type);
}

}
}
}

#endif
