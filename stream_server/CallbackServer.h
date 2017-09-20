#pragma once

#include <thread>
#include <memory>
#include <mutex>
#include <queue>

#include "StreamServer.h"
#include "Message.h"
#include "NetworkType.h"
#include "CallbackMessage.h"

namespace network {

class StreamServer;

class CallbackServer
{
public:
    using CB_SyncReceivedMsg = std::function<void(CallbackMessageContext&, CallbackMessagePtr)>;
    using CB_SyncReceivedMsgArray = std::unordered_map<int32_t, CB_SyncReceivedMsg>;
    using CB_SyncAccept  = std::function<void(Hdl)>;
    using CB_SyncTimeout = std::function<void(Hdl)>;
    using CB_SyncClosed  = std::function<void(Hdl)>;

public:
    CallbackServer(const std::string& ip, short port);
    ~CallbackServer();
    CallbackServer(const CallbackServer& rhs) = delete;
    CallbackServer& operator=(const CallbackServer& rhs) = delete;
    CallbackServer(CallbackServer&& rhs) = delete;
    CallbackServer& operator=(CallbackServer&& rhs) = delete;

    void start();
    void stop();
    void waitThreadExit();
    void update(DiffTime diff_time);

    void setCB_SyncAccept(CB_SyncAccept cb);
    void setCB_SyncTimeout(CB_SyncTimeout cb);
    void setCB_SyncClosed(CB_SyncClosed cb);
    void setCB_SyncReceivedMessage(CB_SyncReceivedMsgArray arr);

private:
    void syncReceivedMsg();
    void syncTimeout();
    void syncClosed();
    void syncAccept();

    void asyncReceivedMsg(Hdl hdl, std::vector<MessagePtr> msg_list);
    void asyncTimeout(Hdl hdl);
    void asyncClosed(Hdl hdl);
    void asyncAccept(Hdl hdl);

    bool msgCallback(CallbackMessageContext& context, CallbackMessagePtr msg);

private:
    boost::asio::io_service         m_io_service;
    boost::asio::io_service::work   m_work;
    std::thread                     m_thread;
    std::unique_ptr<StreamServer>   m_server;
    CB_SyncReceivedMsgArray         m_sync_received_msg_array;
    CB_SyncAccept                   m_sync_accept;
    CB_SyncTimeout                  m_sync_timeout;
    CB_SyncClosed                   m_sync_closed;

    std::mutex                      m_mtx;
    std::queue<Hdl>                 m_hdl_accept;
    std::queue<Hdl>                 m_hdl_closed;
    std::queue<Hdl>                 m_hdl_timeout;
    std::queue<CallbackMessagePkgPtr> m_received_msg;
};

} // network
