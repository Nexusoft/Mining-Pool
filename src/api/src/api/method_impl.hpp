#ifndef NEXUSPOOL_API_METHOD_IMPL_HPP
#define NEXUSPOOL_API_METHOD_IMPL_HPP

#include "api/method.hpp"
#include "api/shared_data_access.hpp"
#include <json/json.hpp>
#include <memory>
#include <vector>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{
class Method_meta_infos : public Method
{
public:

    Method_meta_infos(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access);
    nlohmann::json execute(Method_params const& params) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_access::Sptr m_data_access;
};

class Method_latest_blocks : public Method
{
public:

    Method_latest_blocks(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access);
    nlohmann::json execute(Method_params const& params) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_access::Sptr m_data_access;

};


class Methods_factory_impl : public Methods_factory
{
public:

    Methods_factory_impl(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access);

    Methods create_api_methods() override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Shared_data_access::Sptr m_data_access;
};




}
}

#endif
