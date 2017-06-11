#pragma once

#include <unordered_map>
#include <functional>

#include "Types.h"

namespace NLNET {
class TSockContext;

using MsgCallbackFunction = std::function<void(TSockContext&, CMessage& msg)>;
using MsgCallbackArray = std::unordered_map<std::string, MsgCallbackFunction>;

using ServiceUpCallback     = std::function<void(TSockContext&)>;
using ServiceDownCallback   = std::function<void(TSockContext&)>;

class CallbackManager
{
public:
    CallbackManager();
    ~CallbackManager();
    CallbackManager(const CallbackManager& rhs) = delete;
    CallbackManager& operator=(const CallbackManager& rhs) = delete;

    bool callbackMsg(TSockContext& sock, CMessage& msg);
    void callbackServiceUp(TSockContext& sock);
    void callbackServiceDown(TSockContext& sock);

    void setMsgCallbackArray(MsgCallbackArray msg_cb_array);

    bool setServiceUpCallback(const std::string& service_name, ServiceUpCallback cb);
    bool setServiceDownCallback(const std::string& service_name, ServiceDownCallback cb);
    void removeServiceUpCallback(const std::string& service_name);
    void removeServiceDownCallback(const std::string& service_name);
private:
    ServiceUpCallback* findUpCallback(const std::string& service_name);
    ServiceDownCallback* findDownCallback(const std::string& service_name);
private:
    MsgCallbackArray m_msg_cb_array;
    std::unordered_map<std::string, ServiceUpCallback> m_up_cbs;
    std::unordered_map<std::string, ServiceDownCallback> m_down_cbs;
};

} // NLNET
