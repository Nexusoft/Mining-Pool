#include "persistance/data_access_impl.hpp"
#include "persistance/command/types.hpp"
#include "persistance/command/command_factory.hpp"
#include <array>

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
	m_create_tables_cmd = m_command_factory->create_command(Type::create_db_schema);
	m_get_banned_ip_cmd = m_command_factory->create_command(Type::get_banned_api_ip);
	m_get_banned_user_ip_cmd = m_command_factory->create_command(Type::get_banned_user_and_ip);
}

bool Data_access_impl::create_tables()
{
	return m_data_storage->execute_command(m_create_tables_cmd);
}

bool Data_access_impl::is_connection_banned(std::string address)
{
	m_get_banned_ip_cmd->set_params(std::move(address));
	return m_data_storage->execute_command(m_get_banned_ip_cmd);
}

bool Data_access_impl::is_user_and_connection_banned(std::string user, std::string address)
{
	std::array<std::string, 2> params{ std::move(user), std::move(address) };
	m_get_banned_user_ip_cmd->set_params(std::move(params));
	return m_data_storage->execute_command(m_get_banned_user_ip_cmd);
}


}
}