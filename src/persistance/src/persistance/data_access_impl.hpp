#ifndef NEXUSPOOL_PERSISTANCE_DATA_ACCESS_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_ACCESS_IMPL_HPP

#include "persistance/data_access.hpp"
#include "persistance/data_storage.hpp"
#include "persistance/command/command.hpp"
#include <spdlog/spdlog.h>
#include <sqlite/sqlite3.h>

#include <memory>
#include <string>

namespace nexuspool
{
namespace persistance
{
    namespace command { class Command_factory; }
    using ::nexuspool::persistance::command::Command;

class Data_access_impl : public Data_access
{
public:

    Data_access_impl(std::shared_ptr<spdlog::logger> logger, 
        persistance::Data_storage::Sptr data_storage, 
        std::shared_ptr<persistance::command::Command_factory> command_factory);

    bool create_tables() override;
    bool is_connection_banned(std::string address) override;
    bool is_user_and_connection_banned(std::string user, std::string address) override;
    bool does_account_exists(std::string account) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    persistance::Data_storage::Sptr m_data_storage;
    std::shared_ptr<persistance::command::Command_factory> m_command_factory;

    // needed commands
    std::shared_ptr<Command> m_create_tables_cmd;
    std::shared_ptr<Command> m_get_banned_ip_cmd;
    std::shared_ptr<Command> m_get_banned_user_ip_cmd;
    std::shared_ptr<Command> m_account_exists_cmd;


};

}
}

#endif
