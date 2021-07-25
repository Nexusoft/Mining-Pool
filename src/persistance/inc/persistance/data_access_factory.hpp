#ifndef NEXUSPOOL_PERSISTANCE_DATA_ACCESS_FACTORY_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_ACCESS_FACTORY_HPP

#include "persistance/data_access.hpp"

#include <memory>
#include <cassert>

namespace nexuspool {
namespace persistance {

class Data_access_factory {
public:

    using Sptr = std::shared_ptr<Data_access_factory>;

    Data_access::Uptr create_data_access();

private:
    virtual Data_access::Uptr create_data_access_impl() = 0;
};


inline Data_access::Uptr Data_access_factory::create_data_access()
{
    // implements
    return create_data_access_impl();
}

}
}

#endif
