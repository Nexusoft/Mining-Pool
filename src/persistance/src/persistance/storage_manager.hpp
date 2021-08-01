#ifndef NEXUSPOOL_PERSISTANCE_STORAGE_MANAGER_HPP
#define NEXUSPOOL_PERSISTANCE_STORAGE_MANAGER_HPP

#include <memory>
#include <any>

namespace nexuspool {
namespace persistance {

class Storage_manager
{
public:
    using Uptr = std::unique_ptr<Storage_manager>;
            
    virtual ~Storage_manager() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    template<typename HandleType> 
    HandleType get_handle() const
    {
        std::any result = get_handle_impl();
        return std::any_cast<HandleType>(result);
    }

private:

    virtual std::any get_handle_impl() const = 0;
};

}
}

#endif
