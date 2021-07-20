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

    template<typename Result, typename CommandType>
    std::shared_ptr<Command<Result, CommandType>> create_command(Type command_type);

private:

    virtual std::any create_command_impl(Type command_type) = 0;
};

template<typename Result, typename CommandType>
inline std::shared_ptr<Command<Result, CommandType>> Command_factory::create_command(Type command_type)
{
    // implements
    std::any result = create_command_impl(command_type);
    return std::any_cast<std::shared_ptr<Command<Result, CommandType>>>(result);
}

}
}
}

#endif
