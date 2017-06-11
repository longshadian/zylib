#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

#include "Types.h"

class DBCallbackManager
{
    using CBFun = std::function<void()>;

public:
    DBCallbackManager();
    ~DBCallbackManager();
    DBCallbackManager(const DBCallbackManager& rhs) = delete;
    DBCallbackManager& operator=(const DBCallbackManager& rhs) = delete;
    DBCallbackManager(DBCallbackManager&& rhs) = delete;
    DBCallbackManager& operator=(DBCallbackManager&& rhs) = delete;

    void cbRspUserData(int32_t uid, CBFun cb);

private:
    std::unordered_map<int32_t, CBFun> m_cb_slots;
};
