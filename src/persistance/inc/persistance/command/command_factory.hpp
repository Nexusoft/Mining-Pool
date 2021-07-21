#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_HPP

#include "persistance/command/command.hpp"

#include <memory>
#include <any>
#include <cassert>

namespace nexuspool {
namespace persistance {
namespace command {

class Command_factory {
public:

    using Sptr = std::shared_ptr<Command_factory>;

    std::any create_command(Type command_type);

private:

    virtual std::any create_command_impl(Type command_type) = 0;
};

inline std::any Command_factory::create_command(Type command_type)
{
    // implements
    std::any result = create_command_impl(command_type);
    return result;
}

}
}
}

#endif
