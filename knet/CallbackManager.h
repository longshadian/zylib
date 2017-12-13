#pragma once

#include <unordered_map>
#include <functional>

#include "knet/KNetTypes.h"

namespace knet {

using Msg_Callback          = std::function<void(ReceivedMsgCtxPtr, ReceivedMsgPtr)>;
using Msg_Callback_Array    = std::unordered_map<int32_t, Msg_Callback>;

class CallbackManager
{
public:
    CallbackManager();
    ~CallbackManager();
    CallbackManager(const CallbackManager& rhs) = delete;
    CallbackManager& operator=(const CallbackManager& rhs) = delete;
    CallbackManager(CallbackManager&& rhs) = delete;
    CallbackManager& operator=(CallbackManager&& rhs) = delete;

    bool                CallbackMsg(ReceivedMsgCtxPtr ctx, ReceivedMsgPtr msg);
    void                SetMsgCallbackArray(Msg_Callback_Array msg_cb_array);
private:
    Msg_Callback_Array  m_msg_cb_array;
};

} // knet
