#include "DBCallbackManager.h"

#include <thread>
#include <chrono>

DBCallbackManager::DBCallbackManager()
{
}

DBCallbackManager::~DBCallbackManager()
{
}

void DBCallbackManager::cbRspUserData(int32_t uid, CBFun cb)
{
    m_cb_slots[uid] = cb;
}
