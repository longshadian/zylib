#include "CallbackClient.h"

#include <boost/format.hpp>

#include "FakeLog.h"

#include "StreamServer.h"
#include "NetworkType.h"
#include "ByteBuffer.h"
#include "CallbackUtility.h"

namespace network {

CallbackClient::CallbackClient(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_work(m_io_service)
    , m_async_client()
    , m_sync_received_msg_array()
    , m_sync_timeout()
    , m_sync_closed()
    , m_sync_connect_success()
    , m_sync_connect_fail()
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
    if (m_async_client)
        return false;

    ClientOption opt{};
    opt.m_read_timeout_seconds = timeout_seconds;
    m_async_client = std::make_shared<StreamClient>(m_io_service, opt);
    m_async_client->setCB_HandlerClosed(std::bind(&CallbackClient::asyncClosed, this, std::placeholders::_1));
    m_async_client->setCB_HandlerTimeout(std::bind(&CallbackClient::asyncTimeout, this, std::placeholders::_1));
    m_async_client->setCB_MessageDecoder(std::bind(&callbackMessageDecoder, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    m_async_client->setCB_ReceivedMessage(std::bind(&CallbackClient::asyncReceivedMsg, this, std::placeholders::_1, std::placeholders::_2));
    return m_async_client->connect(ip, port, std::bind(&CallbackClient::asyncConnect, this, std::placeholders::_1, std::placeholders::_2));
}

bool CallbackClient::reconnect()
{
    /*
     * 重连操作只会在2种情况下发生
     *     [1].connect失败
     *         connect失败时，并未产生RWHandler,而且connect_socket也已经清理。
     *       所以可以复用StreamClient对象进行重新连接
     * 
     *     [2].read write read_timeout write_out或者其他RWHandler的异步操作出错返回
     *         以上几种异步操作出错返回，导致链接断开。此时RWHandler已经被关闭。
     *       而且RWHandler关闭时确保closed或者timeout回掉只会发生一次。确保使用者提供的回掉函数只会别调用一次
     *       
     */
    if (!m_async_client)
        return false;
    return m_async_client->reconnect();
}

void CallbackClient::stop()
{
    if (m_async_client)
        m_async_client->shutdown();
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

void CallbackClient::sendMessage(CallbackMessagePtr msg)
{
    if (m_async_client)
        m_async_client->sendMessage(msg);
}

void CallbackClient::setCB_SyncTimeout(CB_SyncTimeout cb)
{
    m_sync_timeout = std::move(cb);
}

void CallbackClient::setCB_SyncClosed(CB_SyncClosed cb)
{
    m_sync_closed = std::move(cb);
}

void CallbackClient::setCB_SyncConnectSuccess(CB_SyncConnect cb)
{
    m_sync_connect_success = std::move(cb);
}

void CallbackClient::setCB_SyncConnectFail(CB_SyncConnect cb)
{
    m_sync_connect_fail = std::move(cb);
}

void CallbackClient::setCB_SyncReceivedMessage(CB_SyncReceivedMsgArray arr)
{
    m_sync_received_msg_array = std::move(arr);
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

void CallbackClient::syncTimeout()
{
    decltype(m_async_timeout) all_timeout{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_timeout = std::move(m_async_timeout);
    }

    if (all_timeout && m_sync_timeout) {
        m_sync_timeout(*this);
    }
}

void CallbackClient::syncClosed()
{
    decltype(m_async_closed) all_closed{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        all_closed = std::move(m_async_closed);
    }
    if (all_closed && m_sync_closed) {
        m_sync_closed(*this);
    }
}

void CallbackClient::syncConnectSuccess()
{
    decltype(m_async_connect_success) connect_succ{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        connect_succ = std::move(m_async_connect_success);
    }
    if (connect_succ && m_sync_connect_success)
        m_sync_connect_success(*this);
}

void CallbackClient::syncConnectFail()
{
    decltype(m_async_connect_fail) connect_fail{};
    {
        std::lock_guard<std::mutex> lk{ m_mtx };
        connect_fail = std::move(m_async_connect_success);
    }
    if (connect_fail && m_sync_connect_fail)
        m_sync_connect_fail(*this);
}

void CallbackClient::asyncReceivedMsg(Hdl hdl, std::vector<MessagePtr> msg_list)
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

void CallbackClient::asyncTimeout(Hdl hdl)
{
    (void)hdl;
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_async_timeout = m_async_client;
}

void CallbackClient::asyncClosed(Hdl hdl)
{
    (void)hdl;
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_async_closed = m_async_client;
}

void CallbackClient::asyncConnect(boost::system::error_code ec, StreamClient&)
{
    LOG(DEBUG) << "xxxx " << ec;
    if (ec) {
        std::lock_guard<std::mutex> lk{ m_mtx };
        m_async_connect_fail = m_async_client;
    } else {
        std::lock_guard<std::mutex> lk{ m_mtx };
        m_async_connect_success = m_async_client;
    }
}

bool CallbackClient::msgCallback(CallbackMessageContext& context, CallbackMessagePtr msg)
{
    auto it = m_sync_received_msg_array.find(msg->m_msg_id);
    if (it == m_sync_received_msg_array.end())
        return false;
    it->second(context, msg);
    return true;
}

} // network
