#ifndef NEXUSPOOL_PERSISTANCE_DATA_READER_MOCK_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_READER_MOCK_HPP

#include <gmock/gmock.h>
#include "persistance/data_reader.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nexuspool
{
namespace persistance
{

class Data_reader_mock : public Data_reader
{
public:

    MOCK_METHOD(bool, is_connection_banned, (std::string address), (override));
    MOCK_METHOD(bool, is_user_and_connection_banned, (std::string user, std::string address), (override));
    MOCK_METHOD(bool, does_account_exists, (std::string account), (override));
    MOCK_METHOD(Account_data, get_account, (std::string account), (override));
    MOCK_METHOD(std::vector<Account_data_for_payment>, get_active_accounts_from_round, (), (override));
    MOCK_METHOD(std::vector<Block_data>, get_latest_blocks, (), (override));
    MOCK_METHOD(Round_data, get_latest_round, (), (override));
    MOCK_METHOD(Round_data, get_round, (std::uint32_t round), (override));
    MOCK_METHOD(std::vector<Payment_data>, get_payments, (std::string account), (override));
    MOCK_METHOD(Config_data, get_config, (), (override));
    MOCK_METHOD(std::vector<Block_data>, get_blocks_from_round, (std::uint32_t round), (override));
    MOCK_METHOD(double, get_total_shares_from_accounts, (), (override));
    MOCK_METHOD(std::vector<Payment_data>, get_not_paid_data_from_round, (std::uint32_t round), (override));
    MOCK_METHOD(std::vector<std::uint32_t>, get_unpaid_rounds, (), (override));
    MOCK_METHOD(std::vector<std::uint32_t>, get_blocks_without_hash_from_round, (std::uint32_t round), (override));
    MOCK_METHOD(double, get_pool_hashrate, (), (override));
    MOCK_METHOD(Statistics_block_finder, get_longest_chain_finder, (), (override));
};

}
}

#endif
