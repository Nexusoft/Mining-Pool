#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_FACTORY_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_FACTORY_HPP

#include "persistance/data_writer.hpp"

#include <memory>
#include <cassert>

namespace nexuspool {
namespace persistance {

class Data_writer_factory {
public:

    using Sptr = std::shared_ptr<Data_writer_factory>;

    Shared_data_writer::Sptr create_shared_data_writer();

private:
    virtual Shared_data_writer::Sptr create_shared_data_writer_impl() = 0;
};


inline Shared_data_writer::Sptr Data_writer_factory::create_shared_data_writer()
{
    // implements
    return create_shared_data_writer_impl();
}

}
}

#endif
