#ifndef NEXUSPOOL_PERSISTANCE_STORAGE_MANAGER_HPP
#define NEXUSPOOL_PERSISTANCE_STORAGE_MANAGER_HPP

#include "sqlite/sqlite3.h"
#include <spdlog/spdlog.h>
#include <memory>
#include <any>

namespace nexuspool {
namespace persistance {

class Storage_manager
{
public:
    using Sptr = std::shared_ptr<Storage_manager>;
            
    virtual ~Storage_manager() = default;

    template<typename HandleType> 
    HandleType get_handle() const
    {
        std::any result = get_handle_impl();
        return std::any_cast<HandleType>(result);
    }

private:

    virtual std::any get_handle_impl() const = 0;
};

class Storage_manager_sqlite : public Storage_manager
{
public:

    Storage_manager_sqlite(std::shared_ptr<spdlog::logger> logger, std::string db_name)
    {
        sqlite3_initialize();
        auto result = sqlite3_open_v2(db_name.c_str(), &m_handle, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if (result)
        {
            logger->error("Can't open database: {}", sqlite3_errmsg(m_handle));
        }
    }

    ~Storage_manager_sqlite()
    {
        sqlite3_close(m_handle);
        sqlite3_shutdown();
    }


private:

    std::any get_handle_impl() const override
    {
        return m_handle;
    }

    sqlite3* m_handle;

};

}
}

#endif
