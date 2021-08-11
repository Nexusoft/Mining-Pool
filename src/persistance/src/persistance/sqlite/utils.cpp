
#include "persistance/sqlite/utils.hpp"
#include <assert.h>
#include <variant>

namespace nexuspool {
namespace persistance {

Account_data convert_to_account_data(Row_sqlite row)
{
	assert(row.size() == 7U);

	Account_data result{};
	result.m_address = std::get<std::string>(row[0].m_data);
	result.m_connections = std::get<std::int32_t>(row[1].m_data);
	result.m_created_at = std::get<std::string>(row[2].m_data);
	result.m_last_active = std::get<std::string>(row[3].m_data);
	result.m_shares = std::get<double>(row[4].m_data);
	result.m_balance = std::get<double>(row[5].m_data);
	result.m_hashrate = std::get<double>(row[6].m_data);

	return result;
}
Block_data convert_to_block_data(Row_sqlite row)
{
	return Block_data{};
}

}
}
