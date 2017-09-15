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

namespace network {

class AsyncServer;

struct SMessage : public Message
{
    SMessage();
    virtual ~SMessage();

    virtual const void* data() const override;
    virtual size_t size() const override;

    int32_t m_msg_id{};
    std::vector<uint8_t> m_data{};
};

using SMessagePtr = std::shared_ptr<SMessage>;

struct SMessagePkg
{
    ConnectionHdl m_hdl{};
    std::chrono::system_clock::time_point m_tm{};
    std::vector<std::shared_ptr<SMessage>> m_msg_list{};
};

struct MessageContext
{
    ConnectionHdl m_hdl{};
};

class CallbackServer
{
public:
    using CBReceivedMsg = std::function<void(MessageContext&, SMessagePtr)>;
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

    void cbAsyncMessageDecoder(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out);
    void cbAsyncReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> msg_list);
    void cbAsyncTimeout(ConnectionHdl hdl);
    void cbAsyncClosed(ConnectionHdl hdl);
    void cbAsyncAccept(ConnectionHdl hdl);

    bool msgCallback(MessageContext& context, SMessagePtr msg);

private:
    boost::asio::io_service         m_io_service;
    boost::asio::io_service::work   m_work;
    std::thread                     m_thread;
    std::unique_ptr<AsyncServer>    m_server;
    CBReceivedMsgArray              m_cb_sync_received_msg_array;
    CBAccept                        m_cb_sync_accept;
    CBTimeout                       m_cb_sync_timeout;
    CBClosed                        m_cb_sync_closed;

    std::mutex                      m_mtx;
    std::queue<ConnectionHdl>       m_hdl_accept;
    std::queue<ConnectionHdl>       m_hdl_closed;
    std::queue<ConnectionHdl>       m_hdl_timeout;
    std::queue<std::unique_ptr<SMessagePkg>> m_received_msg;
};

} // network
