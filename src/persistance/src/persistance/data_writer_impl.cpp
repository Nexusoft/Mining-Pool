#include "persistance/data_writer_impl.hpp"
#include "persistance/command/types.hpp"
#include "persistance/command/command_factory.hpp"
#include "persistance/sqlite/command/command_impl.hpp"
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
	m_create_tables_cmd = m_command_factory->create_command(Type::create_db_schema);
	m_create_account_cmd = m_command_factory->create_command(Type::create_account);
	m_add_payment_cmd = m_command_factory->create_command(Type::add_payment);
}

bool Data_writer_impl::create_tables()
{
	return m_data_storage->execute_command(m_create_tables_cmd);
}

bool Data_writer_impl::create_account(std::string account)
{
	m_create_account_cmd->set_params(std::move(account));
	return m_data_storage->execute_command(m_create_account_cmd);
}

bool Data_writer_impl::add_payment(std::string account, double amount)
{
	m_add_payment_cmd->set_params(command::Command_add_payment_params{std::move(account), amount});
	return m_data_storage->execute_command(m_add_payment_cmd);
}

// --------------------------------------------------------------------------------------


Shared_data_writer_impl::Shared_data_writer_impl(Data_writer::Uptr data_writer)
	: m_data_writer{std::move(data_writer)}
{}

bool Shared_data_writer_impl::create_tables()
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->create_tables();
}

bool Shared_data_writer_impl::create_account(std::string account)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->create_account(std::move(account));
}

bool Shared_data_writer_impl::add_payment(std::string account, double amount)
{
	std::scoped_lock lock(m_writer_mutex);
	return m_data_writer->add_payment(std::move(account), amount);
}

}
}