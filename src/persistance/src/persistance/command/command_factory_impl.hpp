#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_FACTORY_IMPL_HPP

#include "persistance/command/command_factory.hpp"
#include "persistance/command/command_impl.hpp"
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

    Command_factory_impl(std::shared_ptr<spdlog::logger> logger, config::Config& config, sqlite3* handle)
        : m_logger{std::move(logger)}
        , m_config{ config }
        , m_handle(handle)
    {
        m_commands.emplace(std::make_pair(Type::get_banned_user_and_ip, 
            std::make_shared<Command_banned_user_and_ip_impl<std::vector<std::string>, sqlite3_stmt*>>(m_handle)));
    }


private:
    std::shared_ptr<spdlog::logger> m_logger;
    sqlite3* m_handle;
    config::Config& m_config;
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
