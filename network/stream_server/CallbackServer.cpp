#include "CallbackServer.h"

#include <boost/format.hpp>

#include "FakeLog.h"

#include "StreamServer.h"
#include "NetworkType.h"
#include "ByteBuffer.h"
#include "CallbackUtility.h"

namespace network {

CallbackServer::CallbackServer(const std::string& ip, short port)
    : m_io_service()
    , m_work(m_io_service)
    , m_thread()
    , m_server()
    , m_sync_received_msg_array()
    , m_sync_accept()
    , m_sync_timeout()
    , m_sync_closed()
    , m_mtx()
    , m_hdl_accept()
    , m_hdl_closed()
    , m_hdl_timeout()
    , m_received_msg()
{
    network::ServerOption opt{};
    m_server = std::make_unique<network::StreamServer>(m_io_service, port, opt);

    m_server->setCB_MessageDecoder(std::bind(&callbackMessageDecoder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    m_server->setCB_ReceivedMessage(std::bind(&CallbackServer::asyncReceivedMsg, this, std::placeholders::_1, std::placeholders::_2));
    m_server->setCB_HandlerTimeout(std::bind(&CallbackServer::asyncTimeout, this, std::placeholders::_1));
    m_server->setCB_HandlerClosed(std::bind(&CallbackServer::asyncClosed, this, std::placeholders::_1));
    m_server->setCB_Accept(std::bind(&CallbackServer::asyncAccept, this, std::placeholders::_1));

    // TODO listen local ip
    (void)ip;
}

CallbackServer::~CallbackServer()
{ 
    stop();
    waitThreadExit(); 
}

void CallbackServer::start()
{
    m_server->accept();
    m_thread = std::thread([this] { m_io_service.run(); });
}

void CallbackServer::stop()
{
    m_server->stop();
}

void CallbackServer::waitThreadExit()
{
    if (m_thread.joinable())
        m_thread.join();
}

void CallbackServer::update(DiffTime diff_time)
{
    (void)diff_time;
    syncAccept();
    syncTimeout();
    syncClosed();
    syncReceivedMsg();
}

void CallbackServer::setCB_SyncAccept(CB_SyncAccept cb)
{
    m_sync_accept = std::move(cb);
}

void CallbackServer::setCB_SyncTimeout(CB_SyncTimeout cb)
{
    m_sync_timeout = std::move(cb);
}

void CallbackServer::setCB_SyncClosed(CB_SyncClosed cb)
{
    m_sync_closed = std::move(cb);
}

void CallbackServer::setCB_SyncReceivedMessage(CB_SyncReceivedMsgArray arr)
{
    m_sync_received_msg_array = std::move(arr);
}

void CallbackServer::syncReceivedMsg()
{
    decltype(m_received_msg) all_msg{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_msg = std::move(m_received_msg);
    }

    while (!all_msg.empty()) {
        auto msg_pkg = std::move(all_msg.front());
        all_msg.pop();
        CallbackMessageContext context{};
        context.m_hdl = msg_pkg->m_hdl;
        for (auto& msg : msg_pkg->m_msg_list) {
            auto ret = msgCallback(context, msg);
            if (!ret) {
                LOG(WARNING) << "can't callback msg_id: " << msg->m_msg_id;
            }
        }
    }
}

void CallbackServer::syncTimeout()
{
    decltype(m_hdl_timeout) all_timeout{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_timeout = std::move(m_hdl_timeout);
    }

    if (m_sync_timeout) {
        while (!all_timeout.empty()) {
            auto hdl = std::move(all_timeout.front());
            all_timeout.pop();
            m_sync_timeout(hdl);
        }
    }
}

void CallbackServer::syncClosed()
{
    decltype(m_hdl_closed) all_closed{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_closed = std::move(m_hdl_closed);
    }
    
    if (m_sync_closed) {
        while (!all_closed.empty()) {
            auto hdl = std::move(all_closed.front());
            all_closed.pop();
            m_sync_closed(hdl);
        }
    }
}

void CallbackServer::syncAccept()
{
    decltype(m_hdl_accept) all_accept{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_accept = std::move(m_hdl_accept);
    }
    
    if (m_sync_accept) {
        while (!all_accept.empty()) {
            auto hdl = std::move(all_accept.front());
            all_accept.pop();
            m_sync_accept(hdl);
        }
    }
}

void CallbackServer::asyncReceivedMsg(Hdl hdl, std::vector<MessagePtr> msg_list)
{
    auto msg_pkg = std::make_shared<CallbackMessagePkg>();
    msg_pkg->m_hdl = hdl;
    msg_pkg->m_tm = std::chrono::system_clock::now();
    msg_pkg->m_msg_list.reserve(msg_list.size());
    for (auto& v : msg_list) {
        msg_pkg->m_msg_list.push_back(std::dynamic_pointer_cast<CallbackMessage>(v));
    }
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_received_msg.push(std::move(msg_pkg));
}

void CallbackServer::asyncTimeout(Hdl hdl)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_hdl_timeout.push(std::move(hdl));
}

void CallbackServer::asyncClosed(Hdl hdl)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_hdl_closed.push(std::move(hdl));
}

void CallbackServer::asyncAccept(Hdl hdl)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_hdl_accept.push(std::move(hdl));
}

bool CallbackServer::msgCallback(CallbackMessageContext& context, CallbackMessagePtr msg)
{
    auto it = m_sync_received_msg_array.find(msg->m_msg_id);
    if (it == m_sync_received_msg_array.end())
        return false;
    it->second(context, msg);
    return true;
}

} // network
