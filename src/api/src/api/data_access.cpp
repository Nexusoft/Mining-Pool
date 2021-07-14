#include "api/data_access.hpp"

namespace nexuspool
{
namespace api
{

Data_access::Data_access(persistance::Data_storage::Sptr data_storage)
	: m_data_storage{std::move(data_storage)}
{
	m_data_storage->init();
}

bool Data_access::is_user_banned(std::string user)
{
	return false;
}

bool Data_access::is_connection_banned(std::string address)
{
	return false;
}


}
}