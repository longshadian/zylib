#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_set>
#include <set>

#include "StreamServer.h"
#include "Message.h"
#include "NetworkType.h"
#include "CallbackMessage.h"

namespace network {

class StreamServer;

class CallbackServer
{
public:
    using CBReceivedMsg = std::function<void(CallbackMessageContext&, CallbackMessagePtr)>;
    using CBReceivedMsgArray = std::unordered_map<int32_t, CBReceivedMsg>;

    using CBAccept  = std::function<void(ConnectionHdl)>;
    using CBTimeout = std::function<void(ConnectionHdl)>;
    using CBClosed  = std::function<void(ConnectionHdl)>;

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

    void setCBAccept(CBAccept cb);
    void setCBTimeout(CBTimeout cb);
    void setCBClosed(CBClosed cb);

private:
    void processReceivedMsg();
    void processTimeout();
    void processClosed();
    void processAccept();

    void cbAsyncReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> msg_list);
    void cbAsyncTimeout(ConnectionHdl hdl);
    void cbAsyncClosed(ConnectionHdl hdl);
    void cbAsyncAccept(ConnectionHdl hdl);

    bool msgCallback(CallbackMessageContext& context, CallbackMessagePtr msg);

private:
    boost::asio::io_service         m_io_service;
    boost::asio::io_service::work   m_work;
    std::thread                     m_thread;
    std::unique_ptr<StreamServer>   m_server;
    CBReceivedMsgArray              m_cb_sync_received_msg_array;
    CBAccept                        m_cb_sync_accept;
    CBTimeout                       m_cb_sync_timeout;
    CBClosed                        m_cb_sync_closed;

    std::mutex                      m_mtx;
    std::queue<ConnectionHdl>       m_hdl_accept;
    std::queue<ConnectionHdl>       m_hdl_closed;
    std::queue<ConnectionHdl>       m_hdl_timeout;
    std::queue<CallbackMessagePkgPtr> m_received_msg;
};

} // network
