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

    Command_factory_impl(std::shared_ptr<spdlog::logger> logger, Storage_manager::Uptr storage_manager)
        : m_logger{std::move(logger)}
        , m_storage_manager{std::move(storage_manager)}
    {
        m_commands.emplace(std::make_pair(Type::get_banned_user_and_ip, 
            std::make_shared<Command_banned_user_and_ip_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_banned_api_ip,
            std::make_shared<Command_banned_api_ip_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::account_exists,
            std::make_shared<Command_account_exists_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_account,
            std::make_shared<Command_get_account_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_blocks,
            std::make_shared<Command_get_blocks_impl>(m_storage_manager->get_handle<sqlite3*>())));
        // Write commands
        m_commands.emplace(std::make_pair(Type::create_db_schema,
            std::make_shared<Command_create_db_schema_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::create_account,
            std::make_shared<Command_create_account_impl>(m_storage_manager->get_handle<sqlite3*>())));
    }

    ~Command_factory_impl()
    {
        m_storage_manager->stop();
    }

private:
    std::shared_ptr<spdlog::logger> m_logger;
    Storage_manager::Uptr m_storage_manager;
    std::map<Type, std::any> m_commands;

    Command::Sptr create_command_impl(Type command_type) override
    {
        Command::Sptr result{};
        switch (command_type)
        {
        case Type::get_banned_user_and_ip:
            result = std::any_cast<std::shared_ptr<Command_banned_user_and_ip_impl>>(m_commands[command_type]); 
            break;
        case Type::get_banned_api_ip: 
            result = std::any_cast<std::shared_ptr<Command_banned_api_ip_impl>>(m_commands[command_type]); 
            break;
        case Type::account_exists:
            result = std::any_cast<std::shared_ptr<Command_account_exists_impl>>(m_commands[command_type]);
            break;
        case Type::get_account:
            result = std::any_cast<std::shared_ptr<Command_get_account_impl>>(m_commands[command_type]);
            break;
        case Type::get_blocks:
            result = std::any_cast<std::shared_ptr<Command_get_blocks_impl>>(m_commands[command_type]);
            break;
            // Write commands
        case Type::create_db_schema:
            result = std::any_cast<std::shared_ptr<Command_create_db_schema_impl>>(m_commands[command_type]);
            break;
        case Type::create_account:
            result = std::any_cast<std::shared_ptr<Command_create_account_impl>>(m_commands[command_type]);
            break;
        }
        

       return result;
    }
};
}
}
}

#endif
