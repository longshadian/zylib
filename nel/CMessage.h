#pragma once

#include <vector>
#include <memory>

#include "Types.h"

namespace nlnet {

class CMessage
{
public:
    CMessage(int32_t msg_id);
    virtual ~CMessage();

    virtual void* data() = 0;
    virtual size_t size() = 0;

    int32_t m_msg_id;
};

struct NetworkMessage
{
    NetworkMessage() = default;

    TSockHdl    m_sock_hdl;
    CMessagePtr m_msg;
};

struct NetworkMessageContext
{
    NetworkMessageContext() = default;

    // TODO
    std::string  m_service_name;
    ServiceID    m_service_id;
    TSockPtr     m_sock;
};

} // nlnet
