#ifndef NEXUSPOOL_SHARED_DATA_READER_HPP
#define NEXUSPOOL_SHARED_DATA_READER_HPP

#include "persistance/data_reader.hpp"
#include <memory>
#include <string>
#include <mutex>

namespace spdlog { class logger; }
namespace nexuspool
{
class Shared_data_reader
{
public:

    using Sptr = std::shared_ptr<Shared_data_reader>;

    Shared_data_reader(persistance::Data_reader::Uptr data_reader)
        : m_data_reader{ std::move(data_reader) }
    {}

    bool does_account_exists(std::string account)
    {
        std::scoped_lock lock(m_db_mutex);
        return m_data_reader->does_account_exists(std::move(account));
    }

    persistance::Account_data get_account(std::string account)
    {
        std::scoped_lock lock(m_db_mutex);
        return m_data_reader->get_account(std::move(account));
    }

private:

    persistance::Data_reader::Uptr m_data_reader;
    std::mutex m_db_mutex;

};

}

#endif
