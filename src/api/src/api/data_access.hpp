#ifndef NEXUSPOOL_PERSISTANCE_DATA_ACCESS_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_ACCESS_HPP

#include "persistance/data_storage.hpp"
#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

namespace nexuspool
{
namespace config { class Config; }
namespace api
{

class Data_access
{
public:

    Data_access(persistance::Data_storage::Sptr data_storage);

    bool is_user_banned(std::string user);
    bool is_connection_banned(std::string address);

private:

    persistance::Data_storage::Sptr m_data_storage;
    std::shared_ptr<spdlog::logger> m_logger;
};

}
}

#endif
