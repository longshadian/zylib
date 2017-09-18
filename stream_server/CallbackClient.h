#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "StreamClient.h"
#include "Message.h"
#include "NetworkType.h"
#include "CallbackMessage.h"

namespace network {

class AsyncServer;

class CallbackClient
{
public:
    using CBReceivedMsg = std::function<void(CallbackMessageContext&, CallbackMessagePtr)>;
    using CBReceivedMsgArray = std::unordered_map<int32_t, CBReceivedMsg>;

    using CBConnect = std::function<void(CallbackClient&)>;

    using CBTimeout = std::function<void(ConnectionHdl)>;
    using CBClosed  = std::function<void(ConnectionHdl)>;

public:
    CallbackClient(boost::asio::io_service& io_service);
    ~CallbackClient();

    CallbackClient(const CallbackClient& rhs) = delete;
    CallbackClient& operator=(const CallbackClient& rhs) = delete;
    CallbackClient(CallbackClient&& rhs) = delete;
    CallbackClient& operator=(CallbackClient&& rhs) = delete;

    bool connect(const std::string& ip, uint16_t port, std::time_t timeout_seconds);
    bool reconnect();
    void stop();
    void update(DiffTime diff_time);

    void setCBTimeout(CBTimeout cb);
    void setCBClosed(CBClosed cb);
    void setConnectSuccess(CBConnect cb);
    void setConnectFail(CBConnect cb);

private:
    void syncReceivedMsg();
    void syncTimeout();
    void syncClosed();
    void syncConnectSuccess();
    void syncConnectFail();

    void asyncReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> msg_list);
    void asyncTimeout(ConnectionHdl hdl);
    void asyncClosed(ConnectionHdl hdl);
    void asyncConnect(boost::system::error_code, StreamClient&);

    bool msgCallback(CallbackMessageContext& context, CallbackMessagePtr msg);

private:
    boost::asio::io_service&        m_io_service;
    boost::asio::io_service::work   m_work;
    StreamClientPtr                 m_client;
    CBReceivedMsgArray              m_cb_sync_received_msg_array;
    CBTimeout                       m_cb_sync_timeout;
    CBClosed                        m_cb_sync_closed;
    CBConnect                       m_cb_sync_connect_success;
    CBConnect                       m_cb_sync_connect_fail;

    std::mutex                      m_mtx;
    StreamClientPtr                 m_async_connect_success;
    StreamClientPtr                 m_async_connect_fail;
    StreamClientPtr                 m_async_timeout;
    StreamClientPtr                 m_async_closed;
    std::queue<CallbackMessagePkgPtr> m_async_received_msg_pkg;
};

} // network
