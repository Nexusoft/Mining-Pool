#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_HPP

#include <persistance/types.hpp>
#include <memory>
#include <string>

namespace nexuspool
{
namespace persistance
{

class Data_writer
{
public:

    using Uptr = std::unique_ptr<Data_writer>;

    virtual ~Data_writer() = default;

    virtual bool create_account(std::string account, std::string display_name) = 0;
    virtual bool add_payment(Payment_data data) = 0;
    virtual bool create_round(std::string round_end_date_time) = 0;
    virtual bool update_account(Account_data data) = 0; 
    virtual bool create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool reset_shares_from_accounts() = 0;
    virtual bool add_block(Block_data data) = 0;
    virtual bool update_block_rewards(std::string hash, bool orphan, double reward) = 0;
    virtual bool update_round(Round_data round) = 0;
    virtual bool account_paid(std::uint32_t round_number, std::string account, std::string tx_id) = 0;
    virtual bool update_block_hash(std::uint32_t height, std::string block_hash) = 0;
    virtual bool update_reward_of_payment(double reward, std::string account, std::uint32_t round_number) = 0;
    virtual bool delete_empty_payments() = 0;
    virtual bool update_block_share_difficulty(std::uint32_t height, double share_difficulty) = 0;
    virtual bool create_miner(std::string account, std::string display_name) = 0;
    virtual bool update_miner(Miner_data data) = 0;
};

// Wrapper for unique data_writer. Ensures thread safety
class Shared_data_writer
{
public:

    using Sptr = std::shared_ptr<Shared_data_writer>;

    virtual ~Shared_data_writer() = default;

    virtual bool create_account(std::string account, std::string display_name) = 0;
    virtual bool add_payment(Payment_data data) = 0;
    virtual bool create_round(std::string round_end_date_time) = 0;
    virtual bool update_account(Account_data data) = 0;
    virtual bool create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool reset_shares_from_accounts() = 0;
    virtual bool add_block(Block_data data) = 0;
    virtual bool update_block_rewards(std::string hash, bool orphan, double reward) = 0;
    virtual bool update_round(Round_data round) = 0;
    virtual bool account_paid(std::uint32_t round_number, std::string account, std::string tx_id) = 0;
    virtual bool update_block_hash(std::uint32_t height, std::string block_hash) = 0;
    virtual bool update_reward_of_payment(double reward, std::string account, std::uint32_t round_number) = 0;
    virtual bool delete_empty_payments() = 0;
    virtual bool update_block_share_difficulty(std::uint32_t height, double share_difficulty) = 0;
    virtual bool create_miner(std::string account, std::string display_name) = 0;
    virtual bool update_miner(Miner_data data) = 0;
};
}
}

#endif
