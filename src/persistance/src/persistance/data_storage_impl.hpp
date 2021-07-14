#ifndef NEXUSPOOL_PERSISTANCE_DATA_STORAGE_IMPL_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_STORAGE_IMPL_HPP

#include "persistance/data_storage.hpp"
#include <memory>

namespace nexuspool {
namespace persistance {

class Data_storage_impl : public  Data_storage
{
public:

	Data_storage_impl();

	void init() override;
	void close() override;
	bool execute_command(std::string command) override;

private:

};

}
}

#endif
