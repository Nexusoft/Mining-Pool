#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_TYPES_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_TYPES_HPP

#include <cstdint>

namespace nexuspool {
namespace persistance {
namespace command {

// Classes of a command. Generic will often do nothing, its more kind of a base class
enum class Class : std::uint8_t
{
	generic = 0,
	database,
	database_sqlite,
	lld
};

// List of available commands
enum class Type : std::uint8_t
{
	get_banned_user_and_ip = 0,
	get_banned_api_ip,
	create_db_schema,
	account_exists,
	get_account,
	get_blocks,
	create_account

};



}
}
}

#endif 
