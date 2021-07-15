#ifndef NEXUSPOOL_PERSISTANCE_DATA_STORAGE_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_STORAGE_HPP

#include <memory>
#include <string>

namespace nexuspool {
namespace persistance {

class Data_storage {
public:

	using Sptr = std::shared_ptr<Data_storage>;

	virtual ~Data_storage() = default;

	virtual bool init() = 0;
	virtual void close() = 0;
	virtual bool execute_command(std::string command) = 0;

};

}
}

#endif
