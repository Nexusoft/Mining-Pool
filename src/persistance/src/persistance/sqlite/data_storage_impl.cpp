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
	command::Result_sqlite result;
	auto return_value = exec_statement_with_result(std::any_cast<command::Command_type_sqlite>(casted_command->get_command()), result);
	casted_command->set_result(std::move(result));
	casted_command->reset();	// resets sqlite prepared_stmt and clears the bindings
	return return_value;
}

bool Data_storage_impl::exec_statement_with_result(command::Command_type_sqlite sql_command, command::Result_sqlite& result)
{
	int ret = 0;
	while ((ret = sqlite3_step(sql_command.m_statement)) == SQLITE_ROW)
	{	
		// process row here
		auto column_index = 0U;
		command::Result_sqlite::Result_columns columns;
		for (auto& column : sql_command.m_columns)
		{
			command::Column_sqlite column;
			switch (column.m_type)
			{
			case command::Column_sqlite::string:
			{
				column.m_data = std::string(reinterpret_cast<const char*>(sqlite3_column_text(sql_command.m_statement, column_index)));
				column.m_type = command::Column_sqlite::string;
				break;
			}

			case command::Column_sqlite::int32:
			{
				column.m_data = sqlite3_column_int(sql_command.m_statement, column_index);
				column.m_type = command::Column_sqlite::int32;
				break;
			}
			case command::Column_sqlite::int64:
			{
				column.m_data = sqlite3_column_int64(sql_command.m_statement, column_index);
				column.m_type = command::Column_sqlite::int64;
				break;
			}
			case command::Column_sqlite::double_t:
			{
				column.m_data = sqlite3_column_double(sql_command.m_statement, column_index);
				column.m_type = command::Column_sqlite::double_t;
				break;
			}
			}
			columns.push_back(column);
			column_index++;
		}
		result.m_rows.push_back(std::move(columns));
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