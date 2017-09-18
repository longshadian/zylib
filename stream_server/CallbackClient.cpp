#include "CallbackClient.h"

#include <boost/format.hpp>

#include "FakeLog.h"

#include "StreamServer.h"
#include "NetworkType.h"
#include "MessageContext.h"
#include "ByteBuffer.h"
#include "CallbackUtility.h"

namespace network {

CallbackClient::CallbackClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_work(m_io_service)
    , m_client()
    , m_cb_sync_received_msg_array()
    , m_cb_sync_timeout()
    , m_cb_sync_closed()
    , m_cb_sync_connect_success()
    , m_cb_sync_connect_fail()
    , m_mtx()
    , m_async_connect_success()
    , m_async_connect_fail()
    , m_async_timeout()
    , m_async_closed()
    , m_async_received_msg_pkg()
{
}

CallbackClient::~CallbackClient()
{ 
    stop();
}

bool CallbackClient::connect(const std::string& ip, uint16_t port, std::time_t timeout_seconds)
{
    if (m_client)
        return false;

    ClientOption opt{};
    opt.m_read_timeout_seconds = timeout_seconds;
    m_client = std::make_shared<StreamClient>(m_io_service, opt);
    m_client->setCBClosed(std::bind(&CallbackClient::asyncClosed, this, std::placeholders::_1));
    m_client->setCBTimeout(std::bind(&CallbackClient::asyncTimeout, this, std::placeholders::_1));
    m_client->setCBMessageDecode(std::bind(&callbackMessageDecoder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    m_client->setCBReceiveMsg(std::bind(&CallbackClient::asyncReceivedMsg, this, std::placeholders::_1, std::placeholders::_2));

    return m_client->connect(ip, port, std::bind(&CallbackClient::asyncConnect, this, std::placeholders::_1, std::placeholders::_2));
}

bool CallbackClient::reconnect()
{
    // TODO
    return true;
}

void CallbackClient::stop()
{
    // TODO
}

void CallbackClient::update(DiffTime diff_time)
{
    (void)diff_time;
    syncConnectSuccess();
    syncConnectFail();
    syncTimeout();
    syncClosed();
    syncReceivedMsg();
}

void CallbackClient::setCBTimeout(CBTimeout cb)
{
    m_cb_sync_timeout = std::move(cb);
}

void CallbackClient::setCBClosed(CBClosed cb)
{
    m_cb_sync_closed = std::move(cb);
}

void CallbackClient::setConnectSuccess(CBConnect cb)
{
    m_cb_sync_connect_success = std::move(cb);
}

void CallbackClient::setConnectFail(CBConnect cb)
{
    m_cb_sync_connect_fail = std::move(cb);
}

void CallbackClient::syncReceivedMsg()
{
    decltype(m_async_received_msg_pkg) all_msg{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_msg = std::move(m_async_received_msg_pkg);
    }

    while (!all_msg.empty()) {
        auto msg_pkg = std::move(all_msg.front());
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

void CallbackClient::syncTimeout()
{
    decltype(m_async_timeout) all_timeout{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_timeout = std::move(m_async_timeout);
    }

    if (all_timeout && m_cb_sync_timeout) {
        m_cb_sync_timeout(m_client->getHdl());
    }
}

void CallbackClient::syncClosed()
{
    decltype(m_async_closed) all_closed{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_closed = std::move(m_async_closed);
    }
    if (all_closed && m_cb_sync_closed) {
        m_cb_sync_closed(m_client->getHdl());
    }
}

void CallbackClient::syncConnectSuccess()
{
    decltype(m_async_connect_success) connect_succ{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        connect_succ = std::move(m_async_connect_success);
    }
    if (connect_succ && m_cb_sync_connect_success)
        m_cb_sync_connect_success(*this);
}

void CallbackClient::syncConnectFail()
{
    decltype(m_async_connect_fail) connect_fail{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        connect_fail = std::move(m_async_connect_success);
    }
    if (connect_fail && m_cb_sync_connect_fail)
        m_cb_sync_connect_fail(*this);
}

void CallbackClient::asyncReceivedMsg(ConnectionHdl hdl, std::vector<MessagePtr> msg_list)
{
    auto msg_pkg = std::make_shared<CallbackMessagePkg>();
    msg_pkg->m_hdl = hdl;
    msg_pkg->m_tm = std::chrono::system_clock::now();
    msg_pkg->m_msg_list.reserve(msg_list.size());
    for (auto& v : msg_list) {
        msg_pkg->m_msg_list.push_back(std::dynamic_pointer_cast<CallbackMessage>(v));
    }
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_async_received_msg_pkg.push(std::move(msg_pkg));
}

void CallbackClient::asyncTimeout(ConnectionHdl hdl)
{
    (void)hdl;
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_async_timeout = m_client;
}

void CallbackClient::asyncClosed(ConnectionHdl hdl)
{
    (void)hdl;
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_async_closed = m_client;
}

void CallbackClient::asyncConnect(boost::system::error_code ec, StreamClient&)
{
    if (ec) {
        std::lock_guard<std::mutex> lk{ m_mtx };
        m_async_connect_fail = m_client;
    } else {

        std::lock_guard<std::mutex> lk{ m_mtx };
        m_async_connect_success = m_client;
    }
}

bool CallbackClient::msgCallback(CallbackMessageContext& context, CallbackMessagePtr msg)
{
    auto it = m_cb_sync_received_msg_array.find(msg->m_msg_id);
    if (it == m_cb_sync_received_msg_array.end())
        return false;
    it->second(context, msg);
    return true;
}

} // network
