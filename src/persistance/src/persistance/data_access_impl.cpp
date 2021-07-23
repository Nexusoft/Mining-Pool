#include "persistance/data_access_impl.hpp"
#include "persistance/command/types.hpp"
#include "persistance/command/command_factory.hpp"
#include "persistance/command/command_impl.hpp"

namespace nexuspool
{
namespace persistance
{

using ::nexuspool::persistance::command::Type;

Data_access_impl::Data_access_impl(std::shared_ptr<spdlog::logger> logger, 
	persistance::Data_storage::Sptr data_storage,
	std::shared_ptr<persistance::command::Command_factory> command_factory)
	: m_logger{std::move(logger)}
	, m_data_storage{std::move(data_storage)}
	, m_command_factory{std::move(command_factory)}
{
//	m_create_tables_cmd = std::any_cast<std::shared_ptr<Command<std::string, sqlite3_stmt*>>>(
	//	m_command_factory->create_command(Type::create_db_schema));
	m_get_banned_ip_cmd = std::any_cast<std::shared_ptr<command::Command_create_db_schema_impl<std::vector<std::string>, sqlite3_stmt*>>>(
		m_command_factory->create_command(Type::get_banned_api_ip));
}

bool Data_access_impl::is_connection_banned(std::string address)
{
	m_data_storage->execute_command(m_get_banned_ip_cmd);
	return false;
}


}
}