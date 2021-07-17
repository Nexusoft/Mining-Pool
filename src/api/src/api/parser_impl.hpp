#ifndef NEXUSPOOL_API_PARSER_IMPL_HPP
#define NEXUSPOOL_API_PARSER_IMPL_HPP

#include "api/parser.hpp"
#include "api/types.hpp"
#include "api/method.hpp"
#include <json/jsonrpcpp.hpp>
#include <memory>
#include <vector>

namespace spdlog { class logger; }
namespace nexuspool
{
namespace api
{

class Parser_impl : public Parser
{
public:

    Parser_impl(std::shared_ptr<spdlog::logger> logger, Methods_factory::Sptr methods_factory);

    bool parse(jsonrpcpp::entity_ptr request, jsonrpcpp::Response& response) override;

private:

    std::shared_ptr<spdlog::logger> m_logger;
    Methods m_methods;

};

}
}

#endif
