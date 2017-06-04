#include "CallbackManager.h"

#include "Log.h"

namespace NLNET {
CallbackManager::CallbackManager()
{
}

CallbackManager::~CallbackManager()
{
}

void CallbackManager::setMsgCallbackArray(MsgCallbackArray msg_cb_array)
{
    m_msg_cb_array = std::move(msg_cb_array);
}

bool CallbackManager::callbackMsg(TSockPtr& sock, CMessage& msg)
{
    auto it = m_msg_cb_array.find(msg.m_msg_name);
    if (it == m_msg_cb_array.end()) {
        return false;
    }
    it->second(sock, msg);
    return true;
}

} // NLNET
