#include "persistance/data_writer_impl.hpp"
#include "persistance/command/types.hpp"
#include "persistance/command/command_factory.hpp"
#include "persistance/sqlite/command/command_impl.hpp"
#include "persistance/sqlite/utils.hpp"
#include "common/utils.hpp"
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
	m_reset_shares_from_accounts_cmd = m_command_factory->create_command(Type::reset_shares_from_accounts);
	m_add_block_cmd = m_command_factory->create_command(Type::add_block);
	m_update_block_rewards_cmd = m_command_factory->create_command(Type::update_block_rewards);
	m_update_round_cmd = m_command_factory->create_command(Type::update_round);
	m_account_paid_cmd = m_command_factory->create_command(Type::account_paid);
	m_update_block_hash_cmd = m_command_factory->create_command(Type::update_block_hash);
	m_update_reward_of_payment_cmd = m_command_factory->create_command(Type::update_reward_of_payment);
	m_delete_empty_payments_cmd = m_command_factory->create_command(Type::delete_empty_payments);
}

bool Data_writer_impl::create_account(std::string account)
{
	m_create_account_cmd->set_params(std::move(account));
	return m_data_storage->execute_command(m_create_account_cmd);
}

bool Data_writer_impl::add_payment(Payment_data data)
{
	m_add_payment_cmd->set_params(command::Command_add_payment_params{ std::move(data.m_account), data.m_amount, data.m_shares, data.m_payment_date_time, data.m_round, data.m_tx_id });
	return m_data_storage->execute_command(m_add_payment_cmd);
}

bool Data_writer_impl::create_round(std::string round_end_date_time)
{
	m_create_round_cmd->set_params(std::move(round_end_date_time));
	return m_data_storage->execute_command(m_create_round_cmd);
}

bool Data_writer_impl::update_account(Account_data data)
{
	m_update_account_cmd->set_params(command::Command_update_account_params{ 
		common::get_datetime_string(std::chrono::system_clock::now()),	// take current time as last_active_time
		data.m_connections, 
		data.m_shares,
		data.m_hashrate, 
		std::move(data.m_address) });
	return m_data_storage->execute_command(m_update_account_cmd);
}

bool Data_writer_impl::create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours)
{
	m_create_config_cmd->set_params(command::Command_config_params{difficulty_divider, fee, std::move(mining_mode), round_duration_hours });
	return m_data_storage->execute_command(m_create_config_cmd);
}

bool Data_writer_impl::update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours)
{
	m_update_config_cmd->set_params(command::Command_config_params{difficulty_divider, fee, std::move(mining_mode), round_duration_hours });
	return m_data_storage->execute_command(m_update_config_cmd);
}

bool Data_writer_impl::reset_shares_from_accounts()
{
	return m_data_storage->execute_command(m_reset_shares_from_accounts_cmd);
}

bool Data_writer_impl::add_block(Block_data data)
{
	m_add_block_cmd->set_params(command::Command_add_block_params{
		static_cast<int>(data.m_height),
		std::move(data.m_type),
		data.m_difficulty,
		data.m_orphan,
		std::move(data.m_block_finder),
		data.m_round,
		data.m_mainnet_reward});
	return m_data_storage->execute_command(m_add_block_cmd);
}

bool Data_writer_impl::update_block_rewards(std::string hash, bool orphan, double reward)
{
	m_update_block_rewards_cmd->set_params(command::Command_update_block_reward_params{
	orphan,
	reward,
	std::move(hash)});
	return m_data_storage->execute_command(m_update_block_rewards_cmd);
}

bool Data_writer_impl::update_round(Round_data round)
{
	m_update_round_cmd->set_params(command::Command_update_round_params{
		round.m_round,
		round.m_total_shares,
		round.m_total_rewards,
		static_cast<int>(round.m_blocks),
		static_cast<int>(round.m_connection_count),
		round.m_is_active,
		round.m_is_paid});
	return m_data_storage->execute_command(m_update_round_cmd);
}

bool Data_writer_impl::account_paid(std::uint32_t round_number, std::string account, std::string tx_id)
{
	m_account_paid_cmd->set_params(command::Command_account_paid_params{round_number, std::move(account), std::move(tx_id) });
	return m_data_storage->execute_command(m_account_paid_cmd);
}

bool Data_writer_impl::update_block_hash(std::uint32_t height, std::string block_hash)
{
	m_update_block_hash_cmd->set_params(command::Command_update_block_hash_params{ static_cast<int>(height), std::move(block_hash) });
	return m_data_storage->execute_command(m_update_block_hash_cmd);
}

bool Data_writer_impl::update_reward_of_payment(double reward, std::string account, std::uint32_t round_number)
{
	m_update_reward_of_payment_cmd->set_params(command::Command_update_reward_of_payment_params{ reward, std::move(account), round_number });
	return m_data_storage->execute_command(m_update_reward_of_payment_cmd);
}

bool Data_writer_impl::delete_empty_payments()
{
	return m_data_storage->execute_command(m_delete_empty_payments_cmd);
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

bool Shared_data_writer_impl::create_round(std::string round_end_date_time)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->create_round(std::move(round_end_date_time));
}

bool Shared_data_writer_impl::update_account(Account_data data)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_account(std::move(data));
}

bool Shared_data_writer_impl::create_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->create_config(std::move(mining_mode), fee, difficulty_divider, round_duration_hours);
}

bool Shared_data_writer_impl::update_config(std::string mining_mode, int fee, int difficulty_divider, int round_duration_hours)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_config(std::move(mining_mode), fee, difficulty_divider, round_duration_hours);
}

bool Shared_data_writer_impl::reset_shares_from_accounts()
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->reset_shares_from_accounts();
}

bool Shared_data_writer_impl::add_block(Block_data data)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->add_block(std::move(data));
}

bool Shared_data_writer_impl::update_block_rewards(std::string hash, bool orphan, double reward)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_block_rewards(std::move(hash), orphan, reward);
}

bool Shared_data_writer_impl::update_round(Round_data round)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_round(std::move(round));
}

bool Shared_data_writer_impl::account_paid(std::uint32_t round_number, std::string account, std::string tx_id)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->account_paid(round_number, std::move(account), std::move(tx_id));
}

bool Shared_data_writer_impl::update_block_hash(std::uint32_t height, std::string block_hash)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_block_hash(height, std::move(block_hash));
}

bool Shared_data_writer_impl::update_reward_of_payment(double reward, std::string account, std::uint32_t round_number)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->update_reward_of_payment(reward, std::move(account), round_number);
}

bool Shared_data_writer_impl::delete_empty_payments()
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->delete_empty_payments();
}

}
}