#include "persistance/storage_manager_impl.hpp"
#include <spdlog/spdlog.h>
#include <sqlite/sqlite3.h>

namespace nexuspool {
namespace persistance {

Storage_manager_sqlite::Storage_manager_sqlite(std::shared_ptr<spdlog::logger> logger, std::string db_name)
    : m_logger{ std::move(logger) }
    , m_db_name{ std::move(db_name) }
{
}

void Storage_manager_sqlite::start()
{
    sqlite3_initialize();
    auto result = sqlite3_open_v2(m_db_name.c_str(), &m_handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if (result)
    {
        m_logger->critical("Can't open database: {}", sqlite3_errmsg(m_handle));
        std::exit(1);   // cant do anything without a db
    }
}

void Storage_manager_sqlite::stop()
{
    sqlite3_close(m_handle);
    sqlite3_shutdown();
}

}
}
