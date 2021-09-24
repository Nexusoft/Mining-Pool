
#include "common/utils.hpp"
#include <date.h>
#include <sstream>

namespace nexuspool {
namespace common {

std::string get_datetime_string(std::chrono::system_clock::time_point t)
{
    return date::format(datetime_format, t);
}

std::string get_datetime_string(std::uint32_t t)
{
    date::sys_seconds tp{ std::chrono::seconds{t} };
    return date::format(datetime_format, tp);
}

std::chrono::system_clock::time_point get_timepoint_from_string(std::string const& date, std::string const& format)
{
    std::stringstream ss{ date };
    std::chrono::system_clock::time_point pt;
    ss >> date::parse(format, pt);
    if (ss.fail())
        throw std::runtime_error("Cannot parse date");
    return pt;
}

}
}
