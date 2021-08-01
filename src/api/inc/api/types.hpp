#ifndef NEXUSPOOL_API_TYPES_HPP
#define NEXUSPOOL_API_TYPES_HPP

#include <memory>
#include <map>
#include <string>
#include <json/json.hpp>

namespace nexuspool {
namespace api {
class Method;

using Method_name = std::string;
using Method_params = nlohmann::json;
using Methods = std::map< Method_name, std::unique_ptr<Method>>;

}
}

#endif