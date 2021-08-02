#ifndef NEXUSPOOL_PERSISTANCE_DATA_READER_FACTORY_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_READER_FACTORY_HPP

#include "persistance/data_reader.hpp"

#include <memory>
#include <cassert>

namespace nexuspool {
namespace persistance {

class Data_reader_factory {
public:

    using Sptr = std::shared_ptr<Data_reader_factory>;

    Data_reader::Uptr create_data_reader();

private:
    virtual Data_reader::Uptr create_data_reader_impl() = 0;
};


inline Data_reader::Uptr Data_reader_factory::create_data_reader()
{
    // implements
    return create_data_reader_impl();
}

}
}

#endif
