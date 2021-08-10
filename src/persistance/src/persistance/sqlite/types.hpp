#ifndef NEXUSPOOL_PERSISTANCE_SQLITE_TYPES_HPP
#define NEXUSPOOL_PERSISTANCE_SQLITE_TYPES_HPP

#include <vector>
#include <string>
#include <variant>
#include <cstdint>
#include "sqlite/sqlite3.h"

namespace nexuspool {
namespace persistance {

struct Column_sqlite
{
	enum Datatype :std::uint8_t
	{
		string = 0,
		int32,
		int64,
		double_t
	};

	Datatype m_type;
	std::variant<std::string, std::int32_t, std::int64_t, double> m_data;
};

using Row_sqlite = std::vector<Column_sqlite>;

struct Command_type_sqlite
{
	sqlite3_stmt* m_statement;
	Row_sqlite m_row;
};

struct Result_sqlite
{
	std::vector<Row_sqlite> m_rows;
};


} 
}

#endif
