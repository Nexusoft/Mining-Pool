#ifndef NEXUSPOOL_PERSISTANCE_DATA_READER_FACTORY_MOCK_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_READER_FACTORY_MOCK_HPP

#include <gmock/gmock.h>
#include "persistance/data_reader_factory.hpp"
#include "persistance/data_reader_mock.hpp"

namespace nexuspool {
namespace persistance {

class Data_reader_factory_mock : public Data_reader_factory
{
    MOCK_METHOD(Data_reader_mock::Uptr, create_data_reader_impl, (), (override));
};

}
}

#endif
