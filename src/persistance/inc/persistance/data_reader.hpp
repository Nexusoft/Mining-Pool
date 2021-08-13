#ifndef NEXUSPOOL_PERSISTANCE_DATA_READER_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_READER_HPP

#include "persistance/types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nexuspool
{
namespace persistance
{

class Data_reader
{
public:

    using Uptr = std::unique_ptr<Data_reader>;

    virtual ~Data_reader() = default;

    virtual bool is_connection_banned(std::string address) = 0;
    virtual bool is_user_and_connection_banned(std::string user, std::string address) = 0;
    virtual bool does_account_exists(std::string account) = 0;
    virtual Account_data get_account(std::string account) = 0;
    virtual std::vector<Block_data> get_latest_blocks() = 0;
    virtual Round_data get_latest_round() = 0;
    virtual std::vector<Payment_data> get_payments(std::string account) = 0;
};
}
}

#endif
