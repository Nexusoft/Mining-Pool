#ifndef NEXUSPOOL_PERSISTANCE_STORAGE_MANAGER_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_STORAGE_MANAGER_IMPL_HPP

#include "persistance/storage_manager.hpp"
#include <memory>
#include <string>
#include <any>

struct sqlite3;
namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {

class Storage_manager_sqlite : public Storage_manager
{
public:

    Storage_manager_sqlite(std::shared_ptr<spdlog::logger> logger, std::string db_name, bool readonly = true);

    void start() override;
    void stop() override;

private:

    std::any get_handle_impl() const override
    {
        return m_handle;
    }

    void update_db_schema();

    std::shared_ptr<spdlog::logger> m_logger;
    std::string m_db_name;
    bool m_readonly;
    sqlite3* m_handle;

};

}
}

#endif
