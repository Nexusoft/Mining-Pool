#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_IMPL_HPP

#include "persistance/command/command_factory.hpp"
#include "persistance/command/command_impl.hpp"
#include "persistance/storage_manager.hpp"
#include <spdlog/spdlog.h>
#include "sqlite/sqlite3.h"
#include <map>
#include <any>
#include <memory>
#include <vector>

namespace nexuspool {
namespace persistance {
namespace command {

class Command_factory_impl : public Command_factory
{
public:

    Command_factory_impl(std::shared_ptr<spdlog::logger> logger, Storage_manager::Sptr storage_manager)
        : m_logger{std::move(logger)}
        , m_storage_manager{std::move(storage_manager)}
    {
        m_commands.emplace(std::make_pair(Type::get_banned_user_and_ip, 
            std::make_shared<Command_banned_user_and_ip_impl<std::vector<std::string>, sqlite3_stmt*>>
                (m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_banned_api_ip,
            std::make_shared<Command_banned_api_ip_impl<std::vector<std::string>, sqlite3_stmt*>>
            (m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::create_db_schema,
            std::make_shared<Command_create_db_schema_impl<std::string, sqlite3_stmt*>>
            (m_storage_manager->get_handle<sqlite3*>())));   
    }


private:
    std::shared_ptr<spdlog::logger> m_logger;
    Storage_manager::Sptr m_storage_manager;
    std::map<Type, std::any> m_commands;

    std::any create_command_impl(Type command_type) override
    {
        return m_commands[command_type];
    }
};
}
}
}

#endif
