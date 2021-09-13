
#include "common/utils.hpp"
#include <date.h>

namespace nexuspool {
namespace common {

std::string get_datetime_string(std::chrono::system_clock::time_point t)
{
	return date::format("%F %T", t);	// %F = Equivalent to %Y-%m-%d  %T = Equivalent to %H:%M:%S
}

}
}
