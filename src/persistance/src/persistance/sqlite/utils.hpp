#ifndef NEXUSPOOL_PERSISTANCE_SQLITE_UTILS_HPP
#define NEXUSPOOL_PERSISTANCE_SQLITE_UTILS_HPP

#include "persistance/types.hpp"
#include "persistance/sqlite/types.hpp"
#include "sqlite/sqlite3.h"
#include <chrono>
#include <string>

namespace nexuspool 
{
namespace persistance 
{

Account_data convert_to_account_data(Row_sqlite row);
Block_data convert_to_block_data(Row_sqlite row);
Round_data convert_to_round_data(Row_sqlite row);
Payment_data convert_to_payment_data(Row_sqlite row);
Config_data convert_to_config_data(Row_sqlite row);
Statistics_block_finder convert_to_statistics_block_finder(Row_sqlite row);

}
}
#endif
