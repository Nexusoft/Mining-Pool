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
};
}
}

#endif
