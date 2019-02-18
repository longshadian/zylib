#pragma once

#include <unordered_map>
#include <functional>

#include "Types.h"

namespace nlnet {

class NetworkMessageContext;

using Msg_Callback          = std::function<void(NetworkMessageContext&, CMessagePtr msg)>;
using Msg_Callback_Array    = std::unordered_map<int32_t, Msg_Callback>;

using ServiceUp_Callback    = std::function<void(NetworkMessageContext&)>;
using ServiceDown_Callback  = std::function<void(NetworkMessageContext&)>;

class CallbackManager
{
public:
    CallbackManager();
    ~CallbackManager();
    CallbackManager(const CallbackManager& rhs) = delete;
    CallbackManager& operator=(const CallbackManager& rhs) = delete;

    bool callbackMsg(NetworkMessageContext& sock, CMessagePtr msg);
    void callbackServiceConnect(NetworkMessageContext& sock);
    void callbackServiceDisconnect(NetworkMessageContext& sock);
    void callbackByteToMessage(ByteBuf& buf, std::vector<CMessagePtr>* out);

    void setMsgCallbackArray(Msg_Callback_Array msg_cb_array);

    bool setServiceConnectCallback(const std::string& service_name, ServiceUp_Callback cb);
    bool setServiceDisconnectCallback(const std::string& service_name, ServiceDown_Callback cb);

    void removeServiceConnectCallback(const std::string& service_name);
    void removeServiceDisconnectCallback(const std::string& service_name);

    void setByteToMessageCallback(ByteToMessage_Callback cb);
    ByteToMessage_Callback& getByteToMessageCallback();
private:
    ServiceUp_Callback* findUpCallback(const std::string& service_name);
    ServiceDown_Callback* findDownCallback(const std::string& service_name);
private:
    Msg_Callback_Array m_msg_cb_array;
    std::unordered_map<std::string, ServiceUp_Callback> m_up_cbs;
    std::unordered_map<std::string, ServiceDown_Callback> m_down_cbs;

    ByteToMessage_Callback m_msg_decoder;
};

} // NLNET
