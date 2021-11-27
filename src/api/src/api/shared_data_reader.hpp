#ifndef NEXUSPOOL_API_SHARED_DATA_READER_HPP
#define NEXUSPOOL_API_SHARED_DATA_READER_HPP

#include "persistance/data_reader.hpp"
#include <memory>
#include <vector>
#include <string>
#include <mutex>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{
class Shared_data_reader
{
public:

    using Sptr = std::shared_ptr<Shared_data_reader>;

    Shared_data_reader(persistance::Data_reader::Uptr data_reader)
        : m_data_reader{std::move(data_reader)}
    {}

    bool does_account_exists(std::string account)
    {
        std::scoped_lock lock(m_db_mutex);
        return m_data_reader->does_account_exists(std::move(account));
    }

    std::vector<persistance::Block_data> get_latest_blocks()
    {
        std::scoped_lock lock(m_db_mutex);
        return m_data_reader->get_latest_blocks();
    }

    persistance::Account_data get_account(std::string const& account)
    {
        std::scoped_lock lock(m_db_mutex);
        return m_data_reader->get_account(account);
    }

    persistance::Config_data get_config()
    {
        std::scoped_lock lock(m_db_mutex);
        return m_data_reader->get_config();
    }

private:

    persistance::Data_reader::Uptr m_data_reader;
    std::mutex m_db_mutex;

};

}
}

#endif
