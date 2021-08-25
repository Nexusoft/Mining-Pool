#ifndef NEXUSPOOL_PERSISTANCE_DATA_READER_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_READER_IMPL_HPP

#include "persistance/data_reader.hpp"
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

class Data_reader_impl : public Data_reader
{
public:

    Data_reader_impl(std::shared_ptr<spdlog::logger> logger,
        persistance::Data_storage::Sptr data_storage, 
        std::shared_ptr<persistance::command::Command_factory> command_factory);

    bool is_connection_banned(std::string address) override;
    bool is_user_and_connection_banned(std::string user, std::string address) override;
    bool does_account_exists(std::string account) override;
    Account_data get_account(std::string account) override;
    std::vector<Block_data> get_latest_blocks() override;
    Round_data get_latest_round() override;
    std::vector<Payment_data> get_payments(std::string account) override;
    Config_data get_config() override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    persistance::Data_storage::Sptr m_data_storage;
    std::shared_ptr<persistance::command::Command_factory> m_command_factory;

    // needed commands
    std::shared_ptr<Command> m_get_banned_ip_cmd;
    std::shared_ptr<Command> m_get_banned_user_ip_cmd;
    std::shared_ptr<Command> m_account_exists_cmd;
    std::shared_ptr<Command> m_get_account_cmd;
    std::shared_ptr<Command> m_get_blocks_cmd;
    std::shared_ptr<Command> m_get_latest_round_cmd;
    std::shared_ptr<Command> m_get_payments_cmd;
    std::shared_ptr<Command> m_get_config_cmd;
};

}
}

#endif
