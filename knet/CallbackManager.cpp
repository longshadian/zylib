#include "knet/CallbackManager.h"

#include "knet/Message.h"

namespace knet {

CallbackManager::CallbackManager()
{
}

CallbackManager::~CallbackManager()
{
}

void CallbackManager::SetMsgCallbackArray(MsgCallbackArray msg_cb_array)
{
    m_msg_cb_array = std::move(msg_cb_array);
}

bool CallbackManager::CallbackMsg(ReceivedMsgCtxPtr ctx, ReceivedMsgPtr msg)
{
    auto it = m_msg_cb_array.find(msg->GetMsgID());
    if (it == m_msg_cb_array.end()) {
        return false;
    }
    it->second(ctx, msg);
    return true;
}

} // knet
