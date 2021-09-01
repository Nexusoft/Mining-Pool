#include "persistance/sqlite/data_storage_impl.hpp"
#include "persistance/types.hpp"
#include <spdlog/spdlog.h>

namespace nexuspool
{
namespace persistance
{
namespace sqlite
{
Data_storage_impl::Data_storage_impl(std::shared_ptr<spdlog::logger> logger, std::string db_name)
	: m_logger{std::move(logger)}
	, m_db_name{ std::move(db_name) }
{
}

bool Data_storage_impl::execute_command(std::any command)
{
	auto casted_command = std::any_cast<std::shared_ptr<command::Command>>(command);

	auto sqlite_command = std::any_cast<Command_type_sqlite>(casted_command->get_command());

	bool return_value{ false };
	switch (sqlite_command.m_type)
	{
	case Command_type_sqlite::Type::result:
	{
		Result_sqlite result;
		return_value = exec_statement_with_result(sqlite_command, result);
		casted_command->set_result(std::move(result));
		break;
	}
	case Command_type_sqlite::Type::multiple_statements:
	{
		return_value = exec_statements(sqlite_command);
		break;
	}
	case Command_type_sqlite::Type::no_result:
	default:
	{
		return_value = exec_statement(sqlite_command);
		break;
	}
	}

	casted_command->reset();	// resets sqlite prepared_stmt and clears the bindings
	return return_value;
}

bool Data_storage_impl::exec_statement(Command_type_sqlite sql_command)
{
	auto ret = sqlite3_step(sql_command.m_statements.front());
	if (ret != SQLITE_DONE)
	{
		m_logger->error("Sqlite step failure");
		return false;
	}
	return true;
}

bool Data_storage_impl::exec_statements(Command_type_sqlite sql_command)
{
	for (auto& stmt : sql_command.m_statements)
	{
		auto ret = sqlite3_step(stmt);
		if (ret != SQLITE_DONE)
		{
			m_logger->error("Sqlite step failure");
			return false;
		}
	}

	return true;
}

bool Data_storage_impl::exec_statement_with_result(Command_type_sqlite sql_command, Result_sqlite& result)
{
	int ret = 0;
	while ((ret = sqlite3_step(sql_command.m_statements.front())) == SQLITE_ROW)
	{	
		// process row here
		auto column_index = 0U;
		Row_sqlite row;
		for (auto& column : sql_command.m_row)
		{
			switch (column.m_type)
			{
			case Column_sqlite::string:
			{
				column.m_data = std::string(reinterpret_cast<const char*>(sqlite3_column_text(sql_command.m_statements.front(), column_index)));
				column.m_type = Column_sqlite::string;
				break;
			}

			case Column_sqlite::int32:
			{
				column.m_data = sqlite3_column_int(sql_command.m_statements.front(), column_index);
				column.m_type = Column_sqlite::int32;
				break;
			}
			case Column_sqlite::int64:
			{
				column.m_data = sqlite3_column_int64(sql_command.m_statements.front(), column_index);
				column.m_type = Column_sqlite::int64;
				break;
			}
			case Column_sqlite::double_t:
			{
				column.m_data = sqlite3_column_double(sql_command.m_statements.front(), column_index);
				column.m_type = Column_sqlite::double_t;
				break;
			}
			}
			row.push_back(column);
			column_index++;
		}
		result.m_rows.push_back(std::move(row));
	}
	if (ret != SQLITE_DONE) 
	{
		m_logger->error("Sqlite step failure");
		return false;
	}
	return true;
}

}
}
}