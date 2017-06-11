#include "CallbackManager.h"

#include "Log.h"
#include "TSock.h"

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

bool CallbackManager::callbackMsg(TSockContext& sock, CMessage& msg)
{
    auto it = m_msg_cb_array.find(msg.m_msg_name);
    if (it == m_msg_cb_array.end()) {
        return false;
    }
    it->second(sock, msg);
    return true;
}

void CallbackManager::callbackServiceUp(TSockContext& sock)
{
    auto* cb = findUpCallback(sock.m_service_name);
    if (cb)
        (*cb)(sock);
}

void CallbackManager::callbackServiceDown(TSockContext& sock)
{
    auto* cb = findDownCallback(sock.m_service_name);
    if (cb)
        (*cb)(sock);
}

bool CallbackManager::setServiceUpCallback(const std::string& service_name,
    ServiceUpCallback cb)
{
    auto it = m_up_cbs.insert({service_name, std::move(cb)});
    return it.second;
}

bool CallbackManager::setServiceDownCallback(const std::string& service_name,
    ServiceDownCallback cb)
{
    auto it = m_down_cbs.insert({service_name, std::move(cb)});
    return it.second;
}

void CallbackManager::removeServiceUpCallback(const std::string& service_name)
{
    m_up_cbs.erase(service_name);
}

void CallbackManager::removeServiceDownCallback(const std::string& service_name)
{
    m_down_cbs.erase(service_name);
}

ServiceUpCallback* CallbackManager::findUpCallback(const std::string& service_name)
{
    auto it = m_up_cbs.find(service_name);
    if (it != m_up_cbs.end())
        return &it->second;
    return nullptr;
}

ServiceDownCallback* CallbackManager::findDownCallback(const std::string& service_name)
{
    auto it = m_down_cbs.find(service_name);
    if (it != m_down_cbs.end())
        return &it->second;
    return nullptr;
}

} // NLNET
