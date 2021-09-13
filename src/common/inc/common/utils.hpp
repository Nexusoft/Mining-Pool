#ifndef NEXUSPOOL_COMMON_UTILS_HPP
#define NEXUSPOOL_COMMON_UTILS_HPP

#include <chrono>
#include <string>

namespace nexuspool {
namespace common {

std::string get_datetime_string(std::chrono::system_clock::time_point t);

}
}
#endif
