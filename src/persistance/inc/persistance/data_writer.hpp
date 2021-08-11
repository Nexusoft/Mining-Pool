#ifndef NEXUSPOOL_PERSISTANCE_DATA_WRITER_HPP
#define NEXUSPOOL_PERSISTANCE_DATA_WRITER_HPP

#include <memory>
#include <string>

namespace nexuspool
{
namespace persistance
{

class Data_writer
{
public:

    using Uptr = std::unique_ptr<Data_writer>;

    virtual ~Data_writer() = default;

    virtual bool create_tables() = 0;
    virtual bool create_account(std::string account) = 0;
};

// Wrapper for unique data_writer. Ensures thread safety
class Shared_data_writer
{
public:

    using Sptr = std::shared_ptr<Shared_data_writer>;

    virtual ~Shared_data_writer() = default;

    virtual bool create_tables() = 0;
    virtual bool create_account(std::string account) = 0;
};
}
}

#endif
