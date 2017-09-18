#pragma once

#include <memory>
#include <vector>

#include "Message.h"
#include "NetworkType.h"

namespace network {

struct CallbackMessage : public Message
{
    CallbackMessage();
    virtual ~CallbackMessage();

    virtual const void* data() const override;
    virtual size_t size() const override;

    int32_t m_msg_id{};
    std::vector<uint8_t> m_data{};
};

using CallbackMessagePtr = std::shared_ptr<CallbackMessage>;

struct CallbackMessagePkg
{
    ConnectionHdl m_hdl{};
    std::chrono::system_clock::time_point   m_tm{};
    std::vector<CallbackMessagePtr>         m_msg_list{};
};
using CallbackMessagePkgPtr = std::shared_ptr<CallbackMessagePkg>;

struct CallbackMessageContext
{
    ConnectionHdl m_hdl{};
};

} // network
