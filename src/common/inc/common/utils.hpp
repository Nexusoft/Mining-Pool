#ifndef NEXUSPOOL_COMMON_UTILS_HPP
#define NEXUSPOOL_COMMON_UTILS_HPP

#include <chrono>
#include <string>

namespace nexuspool {
namespace common {

constexpr const char* datetime_format = { "%Y-%m-%d %H:%M:%S" };

std::string get_datetime_string(std::chrono::system_clock::time_point t);
std::string get_datetime_string(std::uint32_t t);

std::chrono::system_clock::time_point get_timepoint_from_string(std::string const& date, std::string const& format);


}
}
#endif
