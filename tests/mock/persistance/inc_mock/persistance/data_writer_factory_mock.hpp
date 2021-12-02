#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_FACTORY_MOCK_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_FACTORY_MOCK_HPP

#include <gmock/gmock.h>
#include "persistance/data_writer_factory.hpp"
#include "persistance/data_writer_mock.hpp"

#include <memory>
#include <cassert>

namespace nexuspool {
namespace persistance {

class Data_writer_factory_mock : public Data_writer_factory
{
public:
    MOCK_METHOD(Shared_data_writer_mock::Sptr, create_shared_data_writer_impl, (), (override));
};


}
}

#endif
