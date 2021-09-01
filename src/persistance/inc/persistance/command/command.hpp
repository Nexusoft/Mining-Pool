#ifndef NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_HPP
#define NEXUSPOOL_PERSISTANCE_COMMAND_COMMAND_HPP

#include "persistance/command/types.hpp"
#include <memory>
#include <any>

namespace spdlog { class logger; }
namespace nexuspool {
namespace persistance {
namespace command {

class Command
{
public:
	using Sptr = std::shared_ptr<Command>;

	virtual ~Command() = default;

	virtual Class get_class() const = 0;
	virtual Type get_type() const = 0;
	virtual void set_params(std::any params) = 0;
	virtual void set_result(std::any result) = 0;
	virtual std::any get_result() const = 0;
	virtual std::any get_command() const = 0;
	virtual void reset() = 0;
};

class Command_base_database : public Command
{
public:
	virtual ~Command_base_database() = default;

	Class get_class() const override { return Class::database; }
	void set_result(std::any result) override { m_result = std::move(result); }
	void set_params(std::any params) override { m_params = std::move(params); }
	std::any get_result() const override { return m_result; }

protected:

	std::any m_result;
	std::any m_params;
};

}
}
}

#endif 
