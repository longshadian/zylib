#pragma once

#include <unordered_map>
#include <functional>

#include "Types.h"

namespace NLNET {

using MsgCallbackFunction = std::function<void(TSockPtr, CMessage& msg)>;
using MsgCallbackArray = std::unordered_map<std::string, MsgCallbackFunction>;

class CallbackManager
{
public:
    CallbackManager();
    ~CallbackManager();
    CallbackManager(const CallbackManager& rhs) = delete;
    CallbackManager& operator=(const CallbackManager& rhs) = delete;

    bool callbackMsg(TSockPtr& sock, CMessage& msg);

    void setMsgCallbackArray(MsgCallbackArray msg_cb_array);
private:
    MsgCallbackArray m_msg_cb_array;
};

} // NLNET
