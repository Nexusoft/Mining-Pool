#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_HPP

#include "persistance/data_reader_factory.hpp"
#include "persistance/data_writer_factory.hpp"

namespace nexuspool {
namespace persistance {

// The persistance component can have multiple data_readers with each data_reader has its own db connection
// There can only be one data_writer
class Component 
{
public:
    using Uptr = std::unique_ptr<Component>;

    virtual ~Component() = default;

    virtual Data_reader_factory::Sptr get_data_reader_factory() = 0;
    virtual Data_writer_factory::Sptr get_data_writer_factory() = 0;

};


}
}

#endif 
