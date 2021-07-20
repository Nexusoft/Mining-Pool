#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_IMPL_HPP

#include "persistance/command/command.hpp"
#include "sqlite/sqlite3.h"
#include <memory>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {
namespace command {


template<typename Result, typename CommandType>
class Command_banned_user_and_ip_impl : Command_banned_user_and_ip<Result, CommandType>
{
public:

	Command_banned_user_and_ip_impl(sqlite3* handle)
		: m_handle{handle}
	{
		sqlite3_prepare_v2(m_handle,
			"SELECT * FROM banned_user_ip WHERE user = ':user' AND ip = ':ip';",
			-1, &m_banned_user_and_ip_stmt, NULL);
	}
	Class get_class() const override { return Class::database_sqlite; }


	CommandType get_command() const override
	{
		return m_banned_user_and_ip_stmt;
	}

private:
	sqlite3* m_handle;
	sqlite3_stmt* m_banned_user_and_ip_stmt;


};

}
}
}

#endif 
