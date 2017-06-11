#include "GSCallbackManager.h"

#include <thread>
#include <chrono>

GSCallbackManager::GSCallbackManager()
{
}

GSCallbackManager::~GSCallbackManager()
{
}

void GSCallbackManager::regCBRspUserData(int32_t uid, CBFun cb)
{
    m_cb_slots[uid] = cb;
}

void GSCallbackManager::callback(int32_t uid, NLNET::CMessage& msg)
{
    auto it = m_cb_slots.find(uid);
    if (it == m_cb_slots.end())
        return;
    it->second(msg);
}
