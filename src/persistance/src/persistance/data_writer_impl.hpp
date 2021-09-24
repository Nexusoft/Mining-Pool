#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_IMPL_HPP

#include "persistance/data_writer.hpp"
#include "persistance/data_storage.hpp"
#include "persistance/command/command.hpp"
#include <spdlog/spdlog.h>
#include <sqlite/sqlite3.h>

#include <memory>
#include <string>
#include <mutex>

namespace nexuspool
{
namespace persistance
{
namespace command { class Command_factory; }
using ::nexuspool::persistance::command::Command;

class Data_writer_impl : public Data_writer
{
public:

    Data_writer_impl(std::shared_ptr<spdlog::logger> logger,
        persistance::Data_storage::Sptr data_storage,
        std::shared_ptr<persistance::command::Command_factory> command_factory);

    bool create_account(std::string account) override;
    bool add_payment(Payment_data data) override;
    bool create_round(std::string round_end_date_time) override;
    bool update_account(Account_data data) override;
    bool create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) override;
    bool update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) override;
    bool reset_shares_from_accounts() override;
    bool add_block(Block_data data) override;
    bool update_block_rewards(std::string hash, bool orphan, double reward) override;
    bool update_round(Round_data round) override;
    bool account_paid(std::uint32_t round_number, std::string account) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    persistance::Data_storage::Sptr m_data_storage;
    std::shared_ptr<persistance::command::Command_factory> m_command_factory;

    // needed commands
    std::shared_ptr<Command> m_create_account_cmd;
    std::shared_ptr<Command> m_add_payment_cmd;
    std::shared_ptr<Command> m_create_round_cmd;    
    std::shared_ptr<Command> m_update_account_cmd;
    std::shared_ptr<Command> m_create_config_cmd;
    std::shared_ptr<Command> m_update_config_cmd;
    std::shared_ptr<Command> m_reset_shares_from_accounts_cmd;
    std::shared_ptr<Command> m_add_block_cmd;
    std::shared_ptr<Command> m_update_block_rewards_cmd;
    std::shared_ptr<Command> m_update_round_cmd;
    std::shared_ptr<Command> m_account_paid_cmd;
 };

class Shared_data_writer_impl : public Shared_data_writer
{
public:

    Shared_data_writer_impl(Data_writer::Uptr data_writer);

    bool create_account(std::string account) override;
    bool add_payment(Payment_data data) override;
    bool create_round(std::string round_end_date_time) override;
    bool update_account(Account_data data) override;
    bool create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) override;
    bool update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) override;
    bool reset_shares_from_accounts() override;
    bool add_block(Block_data data) override;
    bool update_block_rewards(std::string hash, bool orphan, double reward) override;
    bool update_round(Round_data round) override;
    bool account_paid(std::uint32_t round_number, std::string account) override;

private:

    std::mutex m_writer_mutex;
    Data_writer::Uptr m_data_writer;
};

}
}

#endif
