#ifndef NEXUSPOOL_PERSISTANCE_SQLITE_UTILS_HPP
#define NEXUSPOOL_PERSISTANCE_SQLITE_UTILS_HPP

#include "persistance/types.hpp"
#include "persistance/sqlite/types.hpp"
#include "sqlite/sqlite3.h"

namespace nexuspool {
namespace persistance {

Account_data convert_to_account_data(Row_sqlite row);
Block_data convert_to_block_data(Row_sqlite row);

}
}
#endif
