#include "persistance/data_writer_impl.hpp"
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

Data_writer_impl::Data_writer_impl(std::shared_ptr<spdlog::logger> logger,
	persistance::Data_storage::Sptr data_storage,
	std::shared_ptr<persistance::command::Command_factory> command_factory)
	: m_logger{ std::move(logger) }
	, m_data_storage{ std::move(data_storage) }
	, m_command_factory{ std::move(command_factory) }
{
	m_create_account_cmd = m_command_factory->create_command(Type::create_account);
	m_add_payment_cmd = m_command_factory->create_command(Type::add_payment);
	m_create_round_cmd = m_command_factory->create_command(Type::create_round);
	m_update_account_cmd = m_command_factory->create_command(Type::update_account);
	m_create_config_cmd = m_command_factory->create_command(Type::create_config);
	m_update_config_cmd = m_command_factory->create_command(Type::update_config);
}

bool Data_writer_impl::create_account(std::string account)
{
	m_create_account_cmd->set_params(std::move(account));
	return m_data_storage->execute_command(m_create_account_cmd);
}

bool Data_writer_impl::add_payment(Payment_data data)
{
	m_add_payment_cmd->set_params(command::Command_add_payment_params{ std::move(data.m_account), data.m_amount, data.m_shares, data.m_payment_date_time, data.m_round });
	return m_data_storage->execute_command(m_add_payment_cmd);
}

bool Data_writer_impl::create_round()
{
	return m_data_storage->execute_command(m_create_round_cmd);
}

bool Data_writer_impl::update_account(Account_data data)
{
	m_update_account_cmd->set_params(command::Command_update_account_params{ 
		get_datetime_string(std::chrono::system_clock::now()),	// take current time as last_active_time
		data.m_connections, 
		data.m_shares,
		data.m_balance, 
		data.m_hashrate, 
		std::move(data.m_address) });
	return m_data_storage->execute_command(m_update_account_cmd);
}

bool Data_writer_impl::create_config(std::string mining_mode, int fee, int difficulty_divider)
{
	m_create_config_cmd->set_params(command::Command_config_params{fee, difficulty_divider, std::move(mining_mode)});
	return m_data_storage->execute_command(m_create_config_cmd);
}

bool Data_writer_impl::update_config(std::string mining_mode, int fee, int difficulty_divider)
{
	m_update_config_cmd->set_params(command::Command_config_params{ fee, difficulty_divider, std::move(mining_mode) });
	return m_data_storage->execute_command(m_update_config_cmd);
}

// --------------------------------------------------------------------------------------


Shared_data_writer_impl::Shared_data_writer_impl(Data_writer::Uptr data_writer)
	: m_data_writer{ std::move(data_writer) }
{}

bool Shared_data_writer_impl::create_account(std::string account)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->create_account(std::move(account));
}

bool Shared_data_writer_impl::add_payment(Payment_data data)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->add_payment(std::move(data));
}

bool Shared_data_writer_impl::create_round()
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->create_round();
}

bool Shared_data_writer_impl::update_account(Account_data data)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_account(std::move(data));
}

bool Shared_data_writer_impl::create_config(std::string mining_mode, int fee, int difficulty_divider)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->create_config(std::move(mining_mode), fee, difficulty_divider);
}

bool Shared_data_writer_impl::update_config(std::string mining_mode, int fee, int difficulty_divider)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_config(std::move(mining_mode), fee, difficulty_divider);
}

}
}