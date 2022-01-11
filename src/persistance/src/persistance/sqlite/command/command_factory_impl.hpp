#ifndef NEXUSPOOL_PERSISTANCE_SQLITE_COMMAND_COMMAND_FACTORY_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_SQLITE_COMMAND_COMMAND_FACTORY_IMPL_HPP

#include "persistance/command/command_factory.hpp"
#include "persistance/sqlite/command/command_impl.hpp"
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
        m_commands.emplace(std::make_pair(Type::get_latest_round,
            std::make_shared<Command_get_latest_round_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_round,
            std::make_shared<Command_get_round_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_payments,
            std::make_shared<Command_get_payments_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_config,
            std::make_shared<Command_get_config_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_active_accounts_from_round,
            std::make_shared<Command_get_active_accounts_from_round_impl>(m_storage_manager->get_handle<sqlite3*>())));     
        m_commands.emplace(std::make_pair(Type::get_blocks_from_round,
            std::make_shared<Command_get_blocks_from_round_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_total_shares_from_accounts,
            std::make_shared<Command_get_total_shares_from_accounts_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_not_paid_data_from_round,
            std::make_shared<Command_get_not_paid_data_from_round_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_unpaid_rounds,
            std::make_shared<Command_get_unpaid_rounds_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_blocks_without_hash_from_round,
            std::make_shared<Command_get_blocks_without_hash_from_round_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_pool_hashrate,
            std::make_shared<Command_get_pool_hashrate_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_longest_chain_finder,
            std::make_shared<Command_get_longest_chain_finder_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::get_top_block_finders,
            std::make_shared<Command_get_top_block_finders_impl>(m_storage_manager->get_handle<sqlite3*>())));
        
        // Write commands
        m_commands.emplace(std::make_pair(Type::create_account,
            std::make_shared<Command_create_account_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::add_payment,
            std::make_shared<Command_add_payment_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::create_round,
            std::make_shared<Command_create_round_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::update_account,
            std::make_shared<Command_update_account_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::create_config,
            std::make_shared<Command_create_config_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::update_config,
            std::make_shared<Command_update_config_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::reset_shares_from_accounts,
            std::make_shared<Command_reset_shares_from_accounts_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::add_block,
            std::make_shared<Command_add_block_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::update_block_rewards,
            std::make_shared<Command_update_block_rewards_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::update_round,
            std::make_shared<Command_update_round_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::account_paid,
            std::make_shared<Command_account_paid_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::update_block_hash,
            std::make_shared<Command_update_block_hash_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::update_reward_of_payment,
            std::make_shared<Command_update_reward_of_payment_impl>(m_storage_manager->get_handle<sqlite3*>())));
        m_commands.emplace(std::make_pair(Type::delete_empty_payments,
            std::make_shared<Command_delete_empty_payments_impl>(m_storage_manager->get_handle<sqlite3*>())));   
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
        case Type::get_latest_round:
            result = std::any_cast<std::shared_ptr<Command_get_latest_round_impl>>(m_commands[command_type]);
            break;
        case Type::get_round:
            result = std::any_cast<std::shared_ptr<Command_get_round_impl>>(m_commands[command_type]);
            break;
        case Type::get_payments:
            result = std::any_cast<std::shared_ptr<Command_get_payments_impl>>(m_commands[command_type]);
            break;      
        case Type::get_config:
            result = std::any_cast<std::shared_ptr<Command_get_config_impl>>(m_commands[command_type]);
            break;
        case Type::get_active_accounts_from_round:
            result = std::any_cast<std::shared_ptr<Command_get_active_accounts_from_round_impl>>(m_commands[command_type]);
            break;
        case Type::get_blocks_from_round:
            result = std::any_cast<std::shared_ptr<Command_get_blocks_from_round_impl>>(m_commands[command_type]);
            break;
        case Type::get_total_shares_from_accounts:
            result = std::any_cast<std::shared_ptr<Command_get_total_shares_from_accounts_impl>>(m_commands[command_type]);
            break;
        case Type::get_not_paid_data_from_round:
            result = std::any_cast<std::shared_ptr<Command_get_not_paid_data_from_round_impl>>(m_commands[command_type]);
            break;
        case Type::get_unpaid_rounds:
            result = std::any_cast<std::shared_ptr<Command_get_unpaid_rounds_impl>>(m_commands[command_type]);
            break;
        case Type::get_blocks_without_hash_from_round:
            result = std::any_cast<std::shared_ptr<Command_get_blocks_without_hash_from_round_impl>>(m_commands[command_type]);
            break;
        case Type::get_pool_hashrate:
            result = std::any_cast<std::shared_ptr<Command_get_pool_hashrate_impl>>(m_commands[command_type]);
            break;
        case Type::get_longest_chain_finder:
            result = std::any_cast<std::shared_ptr<Command_get_longest_chain_finder_impl>>(m_commands[command_type]);
            break;
        case Type::get_top_block_finders:
            result = std::any_cast<std::shared_ptr<Command_get_top_block_finders_impl>>(m_commands[command_type]);
            break;
            
            // Write commands
        case Type::create_account:
            result = std::any_cast<std::shared_ptr<Command_create_account_impl>>(m_commands[command_type]);
            break;
        case Type::add_payment:
            result = std::any_cast<std::shared_ptr<Command_add_payment_impl>>(m_commands[command_type]);
            break;
        case Type::create_round:
            result = std::any_cast<std::shared_ptr<Command_create_round_impl>>(m_commands[command_type]);
            break;    
        case Type::update_account:
            result = std::any_cast<std::shared_ptr<Command_update_account_impl>>(m_commands[command_type]);
            break;
        case Type::create_config:
            result = std::any_cast<std::shared_ptr<Command_create_config_impl>>(m_commands[command_type]);
            break;           
        case Type::update_config:
            result = std::any_cast<std::shared_ptr<Command_update_config_impl>>(m_commands[command_type]);
            break;
        case Type::reset_shares_from_accounts:
            result = std::any_cast<std::shared_ptr<Command_reset_shares_from_accounts_impl>>(m_commands[command_type]);
            break;
        case Type::add_block:
            result = std::any_cast<std::shared_ptr<Command_add_block_impl>>(m_commands[command_type]);
            break;
        case Type::update_block_rewards:
            result = std::any_cast<std::shared_ptr<Command_update_block_rewards_impl>>(m_commands[command_type]);
            break;
        case Type::update_round:
            result = std::any_cast<std::shared_ptr<Command_update_round_impl>>(m_commands[command_type]);
            break;
        case Type::account_paid:
            result = std::any_cast<std::shared_ptr<Command_account_paid_impl>>(m_commands[command_type]);
            break;
        case Type::update_block_hash:
            result = std::any_cast<std::shared_ptr<Command_update_block_hash_impl>>(m_commands[command_type]);
            break;
        case Type::update_reward_of_payment:
            result = std::any_cast<std::shared_ptr<Command_update_reward_of_payment_impl>>(m_commands[command_type]);
            break;   
        case Type::delete_empty_payments:
            result = std::any_cast<std::shared_ptr<Command_delete_empty_payments_impl>>(m_commands[command_type]);
            break;            
        }        

       return result;
    }
};
}
}
}

#endif
