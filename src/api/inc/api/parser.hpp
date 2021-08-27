#ifndef NEXUSPOOL_API_PARSER_HPP
#define NEXUSPOOL_API_PARSER_HPP

#include <json/jsonrpcpp.hpp>
#include <memory>

namespace nexuspool
{
namespace api
{

class Parser
{
public:

    using Uptr = std::unique_ptr<Parser>;
    using Sptr = std::shared_ptr<Parser>;

    virtual ~Parser() = default;

    virtual bool parse(jsonrpcpp::entity_ptr request, jsonrpcpp::Response& response) = 0;

};

}
}

#endif
