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

    virtual bool create_account(std::string account) = 0;
    virtual bool add_payment(Payment_data data) = 0;
    virtual bool create_round(std::string round_end_date_time) = 0;
    virtual bool update_account(Account_data data) = 0; 
    virtual bool create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool reset_shares_from_accounts() = 0;
    virtual bool add_block(Block_data data) = 0;
    virtual bool update_block_rewards(std::string hash, bool orphan, double reward) = 0;
};

// Wrapper for unique data_writer. Ensures thread safety
class Shared_data_writer
{
public:

    using Sptr = std::shared_ptr<Shared_data_writer>;

    virtual ~Shared_data_writer() = default;

    virtual bool create_account(std::string account) = 0;
    virtual bool add_payment(Payment_data data) = 0;
    virtual bool create_round(std::string round_end_date_time) = 0;
    virtual bool update_account(Account_data data) = 0;
    virtual bool create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours) = 0;
    virtual bool reset_shares_from_accounts() = 0;
    virtual bool add_block(Block_data data) = 0;
    virtual bool update_block_rewards(std::string hash, bool orphan, double reward) = 0;
};
}
}

#endif
