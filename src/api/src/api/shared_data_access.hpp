#ifndef NEXUSPOOL_API_SHARED_DATA_ACCESS_HPP
#define NEXUSPOOL_API_SHARED_DATA_ACCESS_HPP

#include "persistance/data_access.hpp"
#include <memory>
#include <vector>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{
class Shared_data_access
{
public:

    using Sptr = std::shared_ptr<Shared_data_access>;

    Shared_data_access(persistance::Data_access::Uptr data_access)
    {}

private:


};

}
}

#endif
