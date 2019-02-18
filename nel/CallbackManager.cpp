#include "CallbackManager.h"

#include "Log.h"
#include "TSock.h"
#include "CMessage.h"

namespace nlnet {
CallbackManager::CallbackManager()
{
}

CallbackManager::~CallbackManager()
{
}

void CallbackManager::setMsgCallbackArray(Msg_Callback_Array msg_cb_array)
{
    m_msg_cb_array = std::move(msg_cb_array);
}

bool CallbackManager::callbackMsg(NetworkMessageContext& sock, CMessagePtr msg)
{
    auto it = m_msg_cb_array.find(msg->m_msg_id);
    if (it == m_msg_cb_array.end()) {
        return false;
    }
    it->second(sock, msg);
    return true;
}

void CallbackManager::callbackServiceConnect(NetworkMessageContext& sock)
{
    auto* cb = findUpCallback(sock.m_service_name);
    if (cb)
        (*cb)(sock);
}

void CallbackManager::callbackServiceDisconnect(NetworkMessageContext& sock)
{
    auto* cb = findDownCallback(sock.m_service_name);
    if (cb)
        (*cb)(sock);
}

void CallbackManager::callbackByteToMessage(ByteBuf& buf, std::vector<CMessagePtr>* out)
{
    m_msg_decoder(buf, out);
}

bool CallbackManager::setServiceConnectCallback(const std::string& service_name,
    ServiceUp_Callback cb)
{
    auto it = m_up_cbs.insert({service_name, std::move(cb)});
    return it.second;
}

bool CallbackManager::setServiceDisconnectCallback(const std::string& service_name,
    ServiceDown_Callback cb)
{
    auto it = m_down_cbs.insert({service_name, std::move(cb)});
    return it.second;
}

void CallbackManager::removeServiceConnectCallback(const std::string& service_name)
{
    m_up_cbs.erase(service_name);
}

void CallbackManager::removeServiceDisconnectCallback(const std::string& service_name)
{
    m_down_cbs.erase(service_name);
}

ServiceUp_Callback* CallbackManager::findUpCallback(const std::string& service_name)
{
    auto it = m_up_cbs.find(service_name);
    if (it != m_up_cbs.end())
        return &it->second;
    return nullptr;
}

ServiceDown_Callback* CallbackManager::findDownCallback(const std::string& service_name)
{
    auto it = m_down_cbs.find(service_name);
    if (it != m_down_cbs.end())
        return &it->second;
    return nullptr;
}

void CallbackManager::setByteToMessageCallback(ByteToMessage_Callback cb)
{
    m_msg_decoder = std::move(cb);
}

ByteToMessage_Callback& CallbackManager::getByteToMessageCallback()
{
    return m_msg_decoder;
}

} // NLNET
