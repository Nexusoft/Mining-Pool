#include "api/data_access_impl.hpp"

namespace nexuspool
{
namespace api
{

Data_access_impl::Data_access_impl(std::shared_ptr<spdlog::logger> logger, persistance::Data_storage::Sptr data_storage)
	: m_logger{std::move(logger)}
	, m_data_storage{std::move(data_storage)}
{
	m_data_storage->init();
}

bool Data_access_impl::is_user_banned(std::string user)
{
	return false;
}

bool Data_access_impl::is_connection_banned(std::string address)
{
	return false;
}


}
}