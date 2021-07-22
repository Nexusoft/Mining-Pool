#ifndef NEXUSPOOL_API_DATA_ACCESS_IMPL_HPP
#define NEXUSPOOL_API_DATA_ACCESS_IMPL_HPP

#include "api/data_access.hpp"
#include "persistance/data_storage.hpp"
#include "persistance/command/command.hpp"
#include <spdlog/spdlog.h>
#include <sqlite/sqlite3.h>

#include <memory>
#include <string>

namespace nexuspool
{
namespace persistance { namespace command { class Command_factory; } }
namespace api
{
    using ::nexuspool::persistance::command::Command;
class Data_access_impl : public Data_access
{
public:

    Data_access_impl(std::shared_ptr<spdlog::logger> logger, 
        persistance::Data_storage::Sptr data_storage, 
        std::shared_ptr<persistance::command::Command_factory> command_factory);

    bool is_connection_banned(std::string address) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    persistance::Data_storage::Sptr m_data_storage;
    std::shared_ptr<persistance::command::Command_factory> m_command_factory;

    // needed commands
    std::shared_ptr<Command<std::vector<std::string>, sqlite3_stmt*>> m_create_tables_cmd;
    std::shared_ptr<Command<std::vector<std::string>, sqlite3_stmt*>> m_get_banned_ip_cmd;


};

}
}

#endif
