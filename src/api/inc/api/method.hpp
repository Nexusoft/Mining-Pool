#ifndef NEXUSPOOL_API_METHOD_HPP
#define NEXUSPOOL_API_METHOD_HPP

#include "api/types.hpp"
#include <json/json.hpp>
#include <memory>
#include <string>

namespace nexuspool
{
namespace api
{
struct Method_result
{
    nlohmann::json m_result{ nullptr };
    bool m_is_error{ false };
    std::string m_error_message{};
    int m_error_code{ -1 };

};

class Method
{
public:

    using Uptr = std::unique_ptr<Method>;

    virtual ~Method() = default;

    virtual Method_result execute(Method_params const& params) = 0;
};


class Methods_factory 
{
public:

    using Sptr = std::shared_ptr<Methods_factory>;

    virtual ~Methods_factory() = default;
    virtual Methods create_api_methods() = 0;
};


}
}

#endif
