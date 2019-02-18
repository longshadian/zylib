#pragma once

#include <unordered_map>
#include <functional>

#include "knet/KNetTypes.h"

namespace knet {

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
    void                SetMsgCallbackArray(MsgCallbackArray msg_cb_array);
private:
    MsgCallbackArray  m_msg_cb_array;
};

} // knet
