#ifndef NEXUSPOOL_PERSISTANCE_DATA_ACCESS_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_ACCESS_HPP

#include <memory>
#include <string>

namespace nexuspool
{
namespace persistance
{

class Data_access
{
public:

    using Uptr = std::unique_ptr<Data_access>;

    virtual ~Data_access() = default;

    virtual bool create_tables() = 0;
    virtual bool is_connection_banned(std::string address) = 0;
    virtual bool is_user_and_connection_banned(std::string user, std::string address) = 0;
};
}
}

#endif
