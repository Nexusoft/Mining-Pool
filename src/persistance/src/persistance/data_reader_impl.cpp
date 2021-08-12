#include "persistance/data_reader_impl.hpp"
#include "persistance/command/types.hpp"
#include "persistance/command/command_factory.hpp"
#include "persistance/sqlite/command/command_impl.hpp"
#include "persistance/sqlite/utils.hpp"
#include <array>

namespace nexuspool
{
namespace persistance
{

using ::nexuspool::persistance::command::Type;

Data_reader_impl::Data_reader_impl(std::shared_ptr<spdlog::logger> logger,
	persistance::Data_storage::Sptr data_storage,
	std::shared_ptr<persistance::command::Command_factory> command_factory)
	: m_logger{std::move(logger)}
	, m_data_storage{std::move(data_storage)}
	, m_command_factory{std::move(command_factory)}
{
	m_get_banned_ip_cmd = m_command_factory->create_command(Type::get_banned_api_ip);
	m_get_banned_user_ip_cmd = m_command_factory->create_command(Type::get_banned_user_and_ip);
	m_account_exists_cmd = m_command_factory->create_command(Type::account_exists);
	m_get_account_cmd = m_command_factory->create_command(Type::get_account);
	m_get_blocks_cmd = m_command_factory->create_command(Type::get_blocks);
}

bool Data_reader_impl::is_connection_banned(std::string address)
{
	m_get_banned_ip_cmd->set_params(std::move(address));
	auto return_value = m_data_storage->execute_command(m_get_banned_ip_cmd);
	auto result = std::any_cast<Result_sqlite>(m_get_banned_ip_cmd->get_result());
	return return_value ? (result.m_rows.empty() ? false : true) : false;
}

bool Data_reader_impl::is_user_and_connection_banned(std::string user, std::string address)
{
	std::array<std::string, 2> params{ std::move(user), std::move(address) };
	m_get_banned_user_ip_cmd->set_params(std::move(params));
	auto return_value =  m_data_storage->execute_command(m_get_banned_user_ip_cmd);
	auto result = std::any_cast<Result_sqlite>(m_get_banned_user_ip_cmd->get_result());
	return return_value ? (result.m_rows.empty() ? false : true) : false;
}

bool Data_reader_impl::does_account_exists(std::string account)
{
	m_account_exists_cmd->set_params(std::move(account));
	auto return_value = m_data_storage->execute_command(m_account_exists_cmd);
	auto result = std::any_cast<Result_sqlite>(m_account_exists_cmd->get_result());
	return return_value ? (result.m_rows.empty() ? false : true) : false;
}

Account_data Data_reader_impl::get_account(std::string account)
{
	Account_data account_data{};
	m_get_account_cmd->set_params(std::move(account));
	if (!m_data_storage->execute_command(m_get_account_cmd))
	{
		return account_data;	// return empty result
	}
	auto result = std::any_cast<Result_sqlite>(m_get_account_cmd->get_result());
	auto result_row = result.m_rows.front();
	account_data = convert_to_account_data(std::move(result_row));

	return account_data;
}

std::vector<Block_data> Data_reader_impl::get_latest_blocks()
{
	std::vector<Block_data> blocks{};
	if (!m_data_storage->execute_command(m_get_blocks_cmd))
	{
		return blocks;	// return empty result
	}

	auto result = std::any_cast<Result_sqlite>(m_get_account_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		blocks.push_back(convert_to_block_data(row));
	}

	return blocks;
}


}
}