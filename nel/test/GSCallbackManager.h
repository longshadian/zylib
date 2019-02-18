#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#include "Types.h"

class GSCallbackManager
{
public:
    using CBFun = std::function<void(nlnet::CMessage& msgin)>;
public:
    GSCallbackManager();
    ~GSCallbackManager();
    GSCallbackManager(const GSCallbackManager& rhs) = delete;
    GSCallbackManager& operator=(const GSCallbackManager& rhs) = delete;
    GSCallbackManager(GSCallbackManager&& rhs) = delete;
    GSCallbackManager& operator=(GSCallbackManager&& rhs) = delete;

    void regCBRspUserData(int32_t uid, CBFun cb);
    void callback(int32_t uid, nlnet::CMessage& msg);
private:
    std::unordered_map<int32_t, CBFun> m_cb_slots;
};
