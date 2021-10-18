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
	m_get_latest_round_cmd = m_command_factory->create_command(Type::get_latest_round);
	m_get_round_cmd = m_command_factory->create_command(Type::get_round);
	m_get_payments_cmd = m_command_factory->create_command(Type::get_payments);
	m_get_config_cmd = m_command_factory->create_command(Type::get_config);
	m_get_active_accounts_from_round_cmd = m_command_factory->create_command(Type::get_active_accounts_from_round);
	m_get_blocks_from_round_cmd = m_command_factory->create_command(Type::get_blocks_from_round);
	m_get_total_shares_from_accounts_cmd = m_command_factory->create_command(Type::get_total_shares_from_accounts);
	m_get_not_paid_data_from_round_cmd = m_command_factory->create_command(Type::get_not_paid_data_from_round);
	m_get_unpaid_rounds_cmd = m_command_factory->create_command(Type::get_unpaid_rounds);
	m_get_blocks_without_hash_from_round_cmd = m_command_factory->create_command(Type::get_blocks_without_hash_from_round);
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
	if (!m_data_storage->execute_command(m_account_exists_cmd))
	{
		return false;
	}

	auto result = std::any_cast<Result_sqlite>(m_account_exists_cmd->get_result());
	if (result.m_rows.empty())
	{
		return false;
	}

	auto account_count = std::get<std::int32_t>(result.m_rows.front()[0].m_data);

	return account_count ? true : false;
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
	if (result.m_rows.empty())
	{
		return account_data;	// return empty result
	}
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

	auto result = std::any_cast<Result_sqlite>(m_get_blocks_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		blocks.push_back(convert_to_block_data(std::move(row)));
	}

	return blocks;
}

Round_data Data_reader_impl::get_latest_round()
{
	Round_data round_data{};
	if (!m_data_storage->execute_command(m_get_latest_round_cmd))
	{
		return round_data;	// return empty result
	}
	auto result = std::any_cast<Result_sqlite>(m_get_latest_round_cmd->get_result());
	if (result.m_rows.empty())
	{
		return round_data;	// return empty result
	}
	auto result_row = result.m_rows.front();
	round_data = convert_to_round_data(std::move(result_row));

	return round_data;
}

Round_data Data_reader_impl::get_round(std::uint32_t round)
{
	Round_data round_data{};
	m_get_round_cmd->set_params(static_cast<std::int64_t>(round));
	if (!m_data_storage->execute_command(m_get_round_cmd))
	{
		return round_data;	// return empty result
	}
	auto result = std::any_cast<Result_sqlite>(m_get_round_cmd->get_result());
	if (result.m_rows.empty())
	{
		return round_data;	// return empty result
	}
	auto result_row = result.m_rows.front();
	round_data = convert_to_round_data(std::move(result_row));

	return round_data;
}

std::vector<Payment_data> Data_reader_impl::get_payments(std::string account)
{
	std::vector<Payment_data> payments{};
	m_get_payments_cmd->set_params(std::move(account));
	if (!m_data_storage->execute_command(m_get_payments_cmd))
	{
		return payments;	// return empty result
	}
	auto result = std::any_cast<Result_sqlite>(m_get_payments_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		payments.push_back(convert_to_payment_data(std::move(row)));
	}

	return payments;
}

Config_data Data_reader_impl::get_config()
{
	Config_data config_data{};
	if (!m_data_storage->execute_command(m_get_config_cmd))
	{
		return config_data;	// return empty result
	}
	auto result = std::any_cast<Result_sqlite>(m_get_config_cmd->get_result());
	if (result.m_rows.empty())
	{
		return config_data;	// return empty result
	}

	auto result_row = result.m_rows.front();
	config_data = convert_to_config_data(std::move(result_row));

	return config_data;
}

std::vector<Account_data_for_payment> Data_reader_impl::get_active_accounts_from_round()
{
	std::vector<Account_data_for_payment> accounts_data{};
	if (!m_data_storage->execute_command(m_get_active_accounts_from_round_cmd))
	{
		return accounts_data;	// return empty result
	}
	auto result = std::any_cast<Result_sqlite>(m_get_active_accounts_from_round_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		accounts_data.push_back(Account_data_for_payment{ std::get<std::string>(row[0].m_data), std::get<double>(row[1].m_data) });
	}

	return accounts_data;
}

std::vector<Block_data> Data_reader_impl::get_blocks_from_round(std::uint32_t round)
{
	std::vector<Block_data> blocks{};
	m_get_blocks_from_round_cmd->set_params(static_cast<std::int64_t>(round));
	if (!m_data_storage->execute_command(m_get_blocks_from_round_cmd))
	{
		return blocks;	// return empty result
	}

	auto result = std::any_cast<Result_sqlite>(m_get_blocks_from_round_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		blocks.push_back(convert_to_block_data(std::move(row)));
	}

	return blocks;
}

double Data_reader_impl::get_total_shares_from_accounts()
{
	if (!m_data_storage->execute_command(m_get_total_shares_from_accounts_cmd))
	{
		return 0;
	}

	auto result = std::any_cast<Result_sqlite>(m_get_total_shares_from_accounts_cmd->get_result());
	if (result.m_rows.empty())
	{
		return 0;
	}

	return std::get<std::double_t>(result.m_rows.front()[0].m_data);
}

std::vector<Payment_data> Data_reader_impl::get_not_paid_data_from_round(std::uint32_t round)
{
	std::vector<Payment_data> payments{};
	m_get_not_paid_data_from_round_cmd->set_params(static_cast<std::int64_t>(round));
	if (!m_data_storage->execute_command(m_get_not_paid_data_from_round_cmd))
	{
		return payments;	// return empty result
	}

	auto result = std::any_cast<Result_sqlite>(m_get_not_paid_data_from_round_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		payments.push_back(convert_to_payment_data(std::move(row)));
	}

	return payments;
}

std::vector<std::uint32_t> Data_reader_impl::get_unpaid_rounds()
{
	std::vector<std::uint32_t> rounds{};
	if (!m_data_storage->execute_command(m_get_unpaid_rounds_cmd))
	{
		return rounds;	// return empty result
	}

	auto result = std::any_cast<Result_sqlite>(m_get_unpaid_rounds_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		rounds.push_back(static_cast<std::uint32_t>(std::get<std::int64_t>(row[0].m_data)));
	}

	return rounds;
}

std::vector<std::uint32_t> Data_reader_impl::get_blocks_without_hash_from_round(std::uint32_t round)
{
	std::vector<std::uint32_t> heights{};
	m_get_blocks_without_hash_from_round_cmd->set_params(static_cast<std::int64_t>(round));
	if (!m_data_storage->execute_command(m_get_blocks_without_hash_from_round_cmd))
	{
		return heights;	// return empty result
	}

	auto result = std::any_cast<Result_sqlite>(m_get_blocks_without_hash_from_round_cmd->get_result());
	for (auto& row : result.m_rows)
	{
		heights.push_back(static_cast<std::uint32_t>(std::get<std::int32_t>(row[0].m_data)));
	}

	return heights;
}


}
}