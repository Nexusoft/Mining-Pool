#ifndef NEXUSPOOL_PERSISTANCE_DATA_STORAGE_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_STORAGE_HPP

#include <memory>
#include <string>
#include <any>

namespace nexuspool {
namespace persistance {

class Data_storage {
public:

	using Sptr = std::shared_ptr<Data_storage>;

	virtual ~Data_storage() = default;

	virtual bool execute_command(std::any command) = 0;

};

}
}

#endif
