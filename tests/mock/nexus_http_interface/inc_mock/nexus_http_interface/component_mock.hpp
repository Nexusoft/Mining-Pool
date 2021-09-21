#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_MOCK_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_COMPONENT_MOCK_HPP

#include <gmock/gmock.h>
#include <string>
#include "nexus_http_interface/component.hpp"

namespace nexuspool {
namespace nexus_http_interface {

class Component_mock : public Component
{
public:
    MOCK_METHOD(bool, get_block_reward_data, (std::string hash, common::Block_reward_data& reward_data), (override));
    MOCK_METHOD(bool, payout, (std::string account_from, std::string pin, Payout_recipients const& recipients), (override));

};


}
}

#endif 
