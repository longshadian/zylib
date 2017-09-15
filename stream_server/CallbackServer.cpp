#include "CallbackServer.h"

#include <boost/format.hpp>

#include "FakeLog.h"

#include "StreamServer.h"
#include "NetworkType.h"
#include "MessageContext.h"
#include "ByteBuffer.h"

namespace network {

SMessage::SMessage()
    : Message()
    , m_msg_id()
    , m_data()
{
}

SMessage::~SMessage()
{
}

const void* SMessage::data() const
{
    if (m_data.empty())
        return nullptr;
    return m_data.data();
}

size_t SMessage::size() const
{
    return m_data.size();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CallbackServer::CallbackServer(const std::string& ip, short port)
    : m_io_service()
    , m_work(m_io_service)
    , m_thread()
    , m_server()
    , m_cb_sync_received_msg_array()
    , m_cb_sync_accept()
    , m_cb_sync_timeout()
    , m_cb_sync_closed()
    , m_mtx()
    , m_hdl_accept()
    , m_hdl_closed()
    , m_hdl_timeout()
    , m_received_msg()
{
    network::ServerOption opt{};
    m_server = std::make_unique<network::AsyncServer>(m_io_service, port, opt);
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
    processAccept();
    processTimeout();
    processClosed();
    processReceivedMsg();
}

void CallbackServer::setCBAccept(CBAccept cb)
{
    m_cb_sync_accept = std::move(cb);
}

void CallbackServer::setCBTimeout(CBTimeout cb)
{
    m_cb_sync_timeout = std::move(cb);
}

void CallbackServer::setCBClosed(CBClosed cb)
{
    m_cb_sync_closed = std::move(cb);
}

void CallbackServer::processReceivedMsg()
{
    decltype(m_received_msg) all_msg{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_msg = std::move(m_received_msg);
    }

    while (!all_msg.empty()) {
        auto msg_pkg = std::move(all_msg.front());
        MessageContext context{};
        context.m_hdl = msg_pkg->m_hdl;
        for (auto& msg : msg_pkg->m_msg_list) {
            auto ret = msgCallback(context, msg);
            if (!ret) {
                LOG(WARNING) << "can't callback msg_id: " << msg->m_msg_id;
            }
        }
    }
}

void CallbackServer::processTimeout()
{
    decltype(m_hdl_timeout) all_timeout{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_timeout = std::move(m_hdl_timeout);
    }

    if (m_cb_sync_timeout) {
        while (!all_timeout.empty()) {
            auto hdl = std::move(all_timeout.front());
            all_timeout.pop();
            m_cb_sync_timeout(hdl);
        }
    }
}

void CallbackServer::processClosed()
{
    decltype(m_hdl_closed) all_closed{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_closed = std::move(m_hdl_closed);
    }
    
    if (m_cb_sync_closed) {
        while (!all_closed.empty()) {
            auto hdl = std::move(all_closed.front());
            all_closed.pop();
            m_cb_sync_closed(hdl);
        }
    }
}

void CallbackServer::processAccept()
{
    decltype(m_hdl_accept) all_accept{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_accept = std::move(m_hdl_accept);
    }
    
    if (m_cb_sync_accept) {
        while (!all_accept.empty()) {
            auto hdl = std::move(all_accept.front());
            all_accept.pop();
            m_cb_sync_accept(hdl);
        }
    }
}

void CallbackServer::cbAsyncMessageDecoder(ConnectionHdl hdl, ByteBuffer& buffer, std::vector<MessagePtr>* out)
{
    (void)hdl;
    while (true) {
        auto buffer_size = static_cast<int32_t>(buffer.byteSize());
        if (buffer_size < 8)
            break;

        int32_t len = 0;
        buffer.read(&len);
        if (len < 8) {
            std::string str = boost::str(boost::format("read head len error. %1%") % len);
            throw std::runtime_error(std::move(str));
        }
        if (len <= buffer_size) {
            buffer.readSkip(sizeof(len));

            int32_t msg_id = 0;
            buffer >> msg_id;
            auto msg = std::make_shared<SMessage>();
            msg->m_msg_id = msg_id;
            if (len - 8 > 0) {
                buffer >> msg->m_data;
            }
            out->push_back(msg);
        } else {
            break;
        }
    }
}

void CallbackServer::cbAsyncReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> msg_list)
{
    auto msg_pkg = std::make_unique<SMessagePkg>();
    msg_pkg->m_hdl = hdl;
    msg_pkg->m_tm = std::chrono::system_clock::now();
    msg_pkg->m_msg_list.reserve(msg_list.size());
    for (auto& v : msg_list) {
        msg_pkg->m_msg_list.push_back(std::dynamic_pointer_cast<SMessage>(v));
    }
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_received_msg.push(std::move(msg_pkg));
}

void CallbackServer::cbAsyncTimeout(ConnectionHdl hdl)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_hdl_timeout.push(std::move(hdl));
}

void CallbackServer::cbAsyncClosed(ConnectionHdl hdl)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_hdl_closed.push(std::move(hdl));
}

void CallbackServer::cbAsyncAccept(ConnectionHdl hdl)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_hdl_accept.push(std::move(hdl));
}

bool CallbackServer::msgCallback(MessageContext& context, SMessagePtr msg)
{
    auto it = m_cb_sync_received_msg_array.find(msg->m_msg_id);
    if (it == m_cb_sync_received_msg_array.end())
        return false;
    it->second(context, msg);
    return true;
}

} // network
