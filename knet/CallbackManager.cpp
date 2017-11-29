#include "knet/CallbackManager.h"

#include "knet/Message.h"

namespace knet {

CallbackManager::CallbackManager()
{
}

CallbackManager::~CallbackManager()
{
}

void CallbackManager::SetMsgCallbackArray(Msg_Callback_Array msg_cb_array)
{
    m_msg_cb_array = std::move(msg_cb_array);
}

bool CallbackManager::CallbackMsg(MessageContextPtr context, ReceivedMessagePtr msg)
{
    auto it = m_msg_cb_array.find(msg->GetMsgID());
    if (it == m_msg_cb_array.end()) {
        return false;
    }
    it->second(context, msg);
    return true;
}

} // knet
