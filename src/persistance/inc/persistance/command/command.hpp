#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_HPP

#include "persistance/command/types.hpp"
#include <memory>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {
namespace command {

template<typename Result, typename CommandType>
class Command
{
public:
	using Sptr = std::shared_ptr<Command<Result, CommandType>>;

	virtual ~Command() = default;

	virtual Class get_class() const = 0;
	virtual Type get_type() const = 0;
	virtual void set_result(Result result) = 0;
	virtual Result get_result() const = 0;
	virtual CommandType get_command() const = 0;
};

// ----------------------------------------------------------------------------

template<typename Result, typename CommandType>
class Command_banned_user_and_ip : Command<Result, CommandType>
{
public:

	virtual ~Command_banned_user_and_ip() = default;

	Class get_class() const override { return Class::generic; }
	Type get_type() const override { return Type::get_banned_user_and_ip; }
	Result get_result() const override { return m_result; }
	void set_result(Result result) override { m_result = result; }

protected:

	Result m_result;
};

template<typename Result, typename CommandType>
class Command_create_db_schema : Command<Result, CommandType>
{
public:

	virtual ~Command_create_db_schema() = default;

	Class get_class() const override { return Class::database; }
	Type get_type() const override { return Type::create_db_schema; }
	Result get_result() const override { return m_result; }
	void set_result(Result result) override { m_result = result; }

protected:

	Result m_result;
};

}
}
}

#endif 
