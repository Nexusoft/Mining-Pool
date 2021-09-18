#ifndef NEXUSPOOL_PERSISTANCE_COMPONENT_MOCK_HPP
#define NEXUSPOOL_PERSISTANCE_COMPONENT_MOCK_HPP

#include <gmock/gmock.h>
#include "persistance/component.hpp"
#include "persistance/data_reader_factory_mock.hpp"
#include "persistance/data_writer_factory_mock.hpp"

namespace nexuspool {
namespace persistance {

class Component_mock : public Component
{
public:

    MOCK_METHOD(Data_reader_factory_mock::Sptr, get_data_reader_factory, (), (override));
    MOCK_METHOD(Data_writer_factory_mock::Sptr, get_data_writer_factory, (), (override));
};


}
}

#endif 
