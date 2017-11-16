#pragma once

#include <unordered_map>
#include <functional>

#include "knet/KNetTypes.h"

namespace knet {

class NetworkMessageContext;

using Msg_Callback          = std::function<void(MessageContextPtr, MessagePtr)>;
using Msg_Callback_Array    = std::unordered_map<int32_t, Msg_Callback>;

class CallbackManager
{
public:
    CallbackManager();
    ~CallbackManager();
    CallbackManager(const CallbackManager& rhs) = delete;
    CallbackManager& operator=(const CallbackManager& rhs) = delete;

    bool CallbackMsg(MessageContextPtr context, MessagePtr msg);
    void SetMsgCallbackArray(Msg_Callback_Array msg_cb_array);

private:
    Msg_Callback_Array m_msg_cb_array;
};

} // knet
