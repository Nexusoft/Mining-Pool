#ifndef NEXUSPOOL_API_SHARED_DATA_ACCESS_HPP
#define NEXUSPOOL_API_SHARED_DATA_ACCESS_HPP

#include "persistance/data_access.hpp"
#include <memory>
#include <vector>
#include <string>
#include <mutex>

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
        : m_data_access{std::move(data_access)}
    {}

    bool does_account_exists(std::string account)
    {
        std::scoped_lock lock(m_db_mutex);
        return m_data_access->does_account_exists(std::move(account));
    }

private:

    persistance::Data_access::Uptr m_data_access;
    std::mutex m_db_mutex;

};

}
}

#endif
