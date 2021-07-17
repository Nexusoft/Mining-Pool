#ifndef NEXUSPOOL_API_METHOD_HPP
#define NEXUSPOOL_API_METHOD_HPP

#include "api/types.hpp"
#include <json/json.hpp>
#include <memory>

namespace nexuspool
{
namespace api
{

class Method
{
public:

    using Uptr = std::unique_ptr<Method>;

    virtual ~Method() = default;

    virtual nlohmann::json execute(Method_params const& params) = 0;
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
