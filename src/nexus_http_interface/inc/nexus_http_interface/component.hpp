#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_HPP

#include "common/types.hpp"
#include <memory>
#include <string>

namespace nexuspool {
namespace nexus_http_interface {

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual bool get_block_reward_data(std::string hash, common::Block_reward_data& reward_data) = 0;

};


}
}

#endif 
