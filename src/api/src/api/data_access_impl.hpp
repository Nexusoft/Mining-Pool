#ifndef NEXUSPOOL_API_DATA_ACCESS_IMPL_HPP
#define NEXUSPOOL_API_DATA_ACCESS_IMPL_HPP

#include "api/data_access.hpp"
#include "persistance/data_storage.hpp"
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace nexuspool
{
namespace config { class Config; }
namespace api
{

class Data_access_impl : public Data_access
{
public:

    Data_access_impl(std::shared_ptr<spdlog::logger> logger, persistance::Data_storage::Sptr data_storage);

    bool is_user_banned(std::string user) override;
    bool is_connection_banned(std::string address) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    persistance::Data_storage::Sptr m_data_storage;

};

}
}

#endif
