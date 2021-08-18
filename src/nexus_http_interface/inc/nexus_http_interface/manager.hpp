#ifndef NEXUSPOOL_NEXUS_HTTP_INTERFACE_MANAGER_HPP
#define NEXUSPOOL_NEXUS_HTTP_INTERFACE_MANAGER_HPP

#include <memory>
#include <cstdint>

namespace nexuspool {
namespace nexus_http_interface {


class Manager {
public:
    using Uptr = std::unique_ptr<Manager>;

    virtual ~Manager() = default;

    //make payments to multiple addresses
    virtual void pay_multiple() const = 0;


};


}
}

#endif 
