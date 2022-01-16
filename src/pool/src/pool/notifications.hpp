#ifndef NEXUSPOOL_NOTIFICATIONS_HPP
#define NEXUSPOOL_NOTIFICATIONS_HPP

#include "pool/session_impl.hpp"

#include <memory>
#include <vector>

namespace nexuspool
{

class Notifications
{
public:

    Notifications(std::shared_ptr<Session_registry> session_registry, bool send_notification)
        : m_session_registry{std::move(session_registry)}
        , m_send_notification{ send_notification }
    {}

    void send_block_found()
    {
        if (!m_send_notification)
        {
            return;
        }
        m_session_registry->send_notification("Pool found a block!");
    }

    void send_pool_shutdown()
    {
        if (!m_send_notification)
        {
            return;
        }
        m_session_registry->send_notification("Pool shutdown in progress.");
    }

private:

    std::shared_ptr<Session_registry> m_session_registry;
    bool m_send_notification;
};
}

#endif
