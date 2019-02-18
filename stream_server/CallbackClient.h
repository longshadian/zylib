#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>

#include "StreamClient.h"
#include "Message.h"
#include "NetworkType.h"
#include "CallbackMessage.h"

namespace network {

class CallbackClient
{
public:
    using CB_SyncReceivedMsg = std::function<void(CallbackMessageContext&, CallbackMessagePtr)>;
    using CB_SyncReceivedMsgArray = std::unordered_map<int32_t, CB_SyncReceivedMsg>;
    using CB_SyncConnect = std::function<void(CallbackClient&)>;
    using CB_SyncTimeout = std::function<void(CallbackClient&)>;
    using CB_SyncClosed  = std::function<void(CallbackClient&)>;

public:
    CallbackClient(boost::asio::io_service& io_service);
    ~CallbackClient();
    CallbackClient(const CallbackClient& rhs) = delete;
    CallbackClient& operator=(const CallbackClient& rhs) = delete;
    CallbackClient(CallbackClient&& rhs) = delete;
    CallbackClient& operator=(CallbackClient&& rhs) = delete;

    bool connect(const std::string& ip, uint16_t port, size_t timeout_seconds = 0);
    bool reconnect();
    void stop();
    void update(DiffTime diff_time);
    void sendMessage(CallbackMessagePtr msg);

    void setCB_SyncTimeout(CB_SyncTimeout cb);
    void setCB_SyncClosed(CB_SyncClosed cb);
    void setCB_SyncConnectSuccess(CB_SyncConnect cb);
    void setCB_SyncConnectFail(CB_SyncConnect cb);
    void setCB_SyncReceivedMessage(CB_SyncReceivedMsgArray arr);

private:
    void syncReceivedMsg();
    void syncTimeout();
    void syncClosed();
    void syncConnectSuccess();
    void syncConnectFail();

    void asyncReceivedMsg(Hdl hdl, std::vector<MessagePtr> msg_list);
    void asyncTimeout(Hdl hdl);
    void asyncClosed(Hdl hdl);
    void asyncConnect(boost::system::error_code, StreamClient&);

    bool msgCallback(CallbackMessageContext& context, CallbackMessagePtr msg);

private:
    boost::asio::io_service&        m_io_service;
    boost::asio::io_service::work   m_work;
    std::string                     m_ip;
    uint16_t                        m_port;
    size_t                          m_read_timeout_seconds;
    StreamClientPtr                 m_async_client;
    CB_SyncReceivedMsgArray              m_sync_received_msg_array;
    CB_SyncTimeout                       m_sync_timeout;
    CB_SyncClosed                        m_sync_closed;
    CB_SyncConnect                       m_sync_connect_success;
    CB_SyncConnect                       m_sync_connect_fail;

    std::mutex                      m_mtx;
    StreamClientPtr                 m_async_connect_success;
    StreamClientPtr                 m_async_connect_fail;
    StreamClientPtr                 m_async_timeout;
    StreamClientPtr                 m_async_closed;
    std::queue<CallbackMessagePkgPtr> m_async_received_msg_pkg;
};

} // network
