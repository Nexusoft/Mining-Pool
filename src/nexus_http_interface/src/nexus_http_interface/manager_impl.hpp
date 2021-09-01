#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_MANAGER_IMPL_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_MANAGER_IMPL_HPP

#include "nexus_http_interface/manager.hpp"
#include <memory>

namespace nexuspool {
namespace nexus_http_interface {

class Manager_impl : public Manager 
{
public:

    Manager_impl();

    void pay_multiple() const override;

private:




};


}
}

#endif 
