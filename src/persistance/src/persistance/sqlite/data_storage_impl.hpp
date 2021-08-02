#ifndef NEXUSPOOL_PERSISTANCE_SQLITE_DATA_STORAGE_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_SQLITE_DATA_STORAGE_IMPL_HPP

#include "persistance/data_storage.hpp"
#include "persistance/command/command_impl.hpp"
#include "sqlite/sqlite3.h"
#include <memory>
#include <string>

namespace spdlog { class logger; }

namespace nexuspool {
namespace persistance {
namespace sqlite {

class Data_storage_impl : public Data_storage
{
public:

	Data_storage_impl(std::shared_ptr<spdlog::logger> logger, std::string db_name);

	bool execute_command(std::any command) override;

private:

	bool exec_statement_with_result(command::Command_type_sqlite sql_command, command::Result_sqlite& result);

	std::shared_ptr<spdlog::logger> m_logger;
	std::string m_db_name;
};

}
}
}

#endif
