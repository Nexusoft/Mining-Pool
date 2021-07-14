#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_HPP

#include "persistance/data_storage_factory.hpp"

#include <memory>

namespace nexuspool {
namespace persistance {

class Component {
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    // Get the Socket_factory interface
    virtual Data_storage_factory::Sptr get_data_storage_factory() = 0;
};


}
}

#endif 
