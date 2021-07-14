#ifndef NEXUSPOOL_PERSISTANCE_DATA_STORAGE_FACTORY_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_STORAGE_FACTORY_HPP

#include "persistance/data_storage.hpp"

#include <memory>
#include <cassert>

namespace nexuspool {
namespace persistance {

class Data_storage_factory {
public:

    using Sptr = std::shared_ptr<Data_storage_factory>;

    Data_storage::Sptr create_data_storage();

private:
    virtual Data_storage::Sptr create_data_storage_impl() = 0;
};


inline Data_storage::Sptr Data_storage_factory::create_data_storage()
{
    // implements
    return create_data_storage_impl();
}

}
}

#endif
