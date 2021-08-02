#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_HPP

#include "persistance/data_reader_factory.hpp"

namespace nexuspool {
namespace persistance {

class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual Data_reader_factory::Sptr get_data_reader_factory() = 0;

};


}
}

#endif 
