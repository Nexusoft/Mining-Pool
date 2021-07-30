#include "api/method_impl.hpp"
#include "TAO/Register/types/address.h"
#include <json/json.hpp>
#include <spdlog/spdlog.h>

namespace nexuspool
{
namespace api
{

Method_meta_infos::Method_meta_infos(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access)
	: m_logger{ std::move(logger) }
    , m_data_access{ std::move(data_access) }
{
}

nlohmann::json Method_meta_infos::execute(Method_params const& params)
{
    // get meta data from data_access
    nlohmann::json result;
    result["pool_hashrate"] = 95.3;
    result["network_hashrate"] = 40.1;
    result["payout_threshold"] = 0.1;
    result["fee"] = 5;

    return result;
}

Method_latest_blocks::Method_latest_blocks(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access)
    : m_logger{ std::move(logger) }
    , m_data_access{ std::move(data_access) }
{
}

nlohmann::json Method_latest_blocks::execute(Method_params const& params)
{
    nlohmann::json result = nlohmann::json::array();
    for (auto i = 0U; i < 100; i++)
    {
        nlohmann::json block;
        block["height"] = i + 10;
        block["hash"] = "randomstring";
        block["block_reward"] = 2.135;
        block["time"] = "2021-07-16 10:12:33.239719";
        block["network_diff"] = 1000.12;
        block["payment_method"] = "test";
        result.push_back(block);
    }

    return result;

}

Method_account::Method_account(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access)
    : m_logger{ std::move(logger) }
    , m_data_access{ std::move(data_access) }
{
}

nlohmann::json Method_account::execute(Method_params const& params)
{
    std::string const account{ params["_account"] };
    // is account a valid nxs address
    TAO::Register::Address address_check{ account };
    if (!address_check.IsValid())
    {
        // return error
    }
    nlohmann::json result = nlohmann::json::array();
    return result;

}

Method_account_header::Method_account_header(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access)
    : m_logger{ std::move(logger) }
    , m_data_access{ std::move(data_access) }
{
}

nlohmann::json Method_account_header::execute(Method_params const& params)
{
    // get meta data from data_access
    nlohmann::json result = nlohmann::json::array();
    return result;

}

Method_account_works::Method_account_works(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access)
    : m_logger{ std::move(logger) }
    , m_data_access{ std::move(data_access) }
{
}

nlohmann::json Method_account_works::execute(Method_params const& params)
{
    // get meta data from data_access
    nlohmann::json result = nlohmann::json::array();
    return result;

}

Method_account_payouts::Method_account_payouts(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access)
    : m_logger{ std::move(logger) }
    , m_data_access{ std::move(data_access) }
{
}

nlohmann::json Method_account_payouts::execute(Method_params const& params)
{
    // get meta data from data_access
    nlohmann::json result = nlohmann::json::array();
    return result;

}

// ----------------------------------------------------------------------------------------------------------------------------

Methods_factory_impl::Methods_factory_impl(std::shared_ptr<spdlog::logger> logger, Shared_data_access::Sptr data_access)
	: m_logger{ std::move(logger) }
    , m_data_access{std::move(data_access)}
{
}

Methods Methods_factory_impl::create_api_methods()
{
    Methods methods;
    methods.emplace(std::make_pair("get_meta_info", std::make_unique<Method_meta_infos>(m_logger, m_data_access)));
    methods.emplace(std::make_pair("get_latest_blocks", std::make_unique<Method_latest_blocks>(m_logger, m_data_access)));
    methods.emplace(std::make_pair("get_account", std::make_unique<Method_account>(m_logger, m_data_access)));
    methods.emplace(std::make_pair("get_account_header", std::make_unique<Method_account_header>(m_logger, m_data_access)));
    methods.emplace(std::make_pair("get_account_works", std::make_unique<Method_account_works>(m_logger, m_data_access)));
    methods.emplace(std::make_pair("get_account_payouts", std::make_unique<Method_account_payouts>(m_logger, m_data_access)));

    return methods;
}


}
}