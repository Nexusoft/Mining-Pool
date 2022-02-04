#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_MOCK_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_MOCK_HPP

#include <gmock/gmock.h>
#include <persistance/types.hpp>
#include <persistance/data_writer.hpp>
#include <memory>
#include <string>

namespace nexuspool
{
namespace persistance
{

class Data_writer_mock : public Data_writer
{
public:

    MOCK_METHOD(bool, create_account, (std::string account, std::string display_name), (override));
    MOCK_METHOD(bool, add_payment, (Payment_data data), (override));
    MOCK_METHOD(bool, create_round, (std::string round_end_date_time), (override));
    MOCK_METHOD(bool, update_account, (Account_data data), (override));
    MOCK_METHOD(bool, create_config, (std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours), (override));
    MOCK_METHOD(bool, update_config, (std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours), (override));
    MOCK_METHOD(bool, reset_shares_from_accounts, (), (override));
    MOCK_METHOD(bool, add_block, (Block_data data), (override));
    MOCK_METHOD(bool, update_block_rewards, (std::string hash, bool orphan, double reward), (override));
    MOCK_METHOD(bool, update_round, (Round_data round), (override));
    MOCK_METHOD(bool, account_paid, (std::uint32_t round_number, std::string account, std::string tx_id), (override));
    MOCK_METHOD(bool, update_block_hash, (std::uint32_t height, std::string block_hash), (override));
    MOCK_METHOD(bool, update_reward_of_payment, (double reward, std::string account, std::uint32_t round_number), (override));
    MOCK_METHOD(bool, delete_empty_payments, (), (override));
    MOCK_METHOD(bool, update_block_share_difficulty, (std::uint32_t height, double share_difficulty), (override));
    MOCK_METHOD(bool, create_miner, (std::string account, std::string display_name), (override));
    MOCK_METHOD(bool, update_miner, (Miner_data data), (override));
};

// Wrapper for unique data_writer. Ensures thread safety
class Shared_data_writer_mock : public Shared_data_writer
{
public:

    MOCK_METHOD(bool, create_account, (std::string account, std::string display_name), (override));
    MOCK_METHOD(bool, add_payment, (Payment_data data), (override));
    MOCK_METHOD(bool, create_round, (std::string round_end_date_time), (override));
    MOCK_METHOD(bool, update_account, (Account_data data), (override));
    MOCK_METHOD(bool, create_config, (std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours), (override));
    MOCK_METHOD(bool, update_config, (std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours), (override));
    MOCK_METHOD(bool, reset_shares_from_accounts, (), (override));
    MOCK_METHOD(bool, add_block, (Block_data data), (override));
    MOCK_METHOD(bool, update_block_rewards, (std::string hash, bool orphan, double reward), (override));
    MOCK_METHOD(bool, update_round, (Round_data round), (override));
    MOCK_METHOD(bool, account_paid, (std::uint32_t round_number, std::string account, std::string tx_id), (override));
    MOCK_METHOD(bool, update_block_hash, (std::uint32_t height, std::string block_hash), (override));
    MOCK_METHOD(bool, update_reward_of_payment, (double reward, std::string account, std::uint32_t round_number), (override));
    MOCK_METHOD(bool, delete_empty_payments, (), (override));
    MOCK_METHOD(bool, update_block_share_difficulty, (std::uint32_t height, double share_difficulty), (override));
    MOCK_METHOD(bool, create_miner, (std::string account, std::string display_name), (override));
    MOCK_METHOD(bool, update_miner, (Miner_data data), (override));
};

}
}

#endif
