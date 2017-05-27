#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "Types.h"

namespace NLNET {

class TSock;

using TSockHdl = std::weak_ptr<TSock>;

struct ConnectionInfo
{
    size_t m_timeout_seconds;
    std::string m_ip;
    std::string m_port;
};

class TSock : public std::enable_shared_from_this<TSock>
{
public:
    using ConnectionAccept = std::function<void(TSockHdl, const ConnectionInfo)>;
    using ConnectionClosed = std::function<void(TSockHdl)>;
    using ConnectionTimeout = std::function<void(TSockHdl)>;
    using ReceivedMsgCallback = std::function<void(NetWorkMessage, TSockHdl)>;

    enum class CLOSED_TYPE : int
    {
        NORMAL   = 0,    //正常关闭
        TIMEOUT  = 1,    //超时关闭
        ACTIVITY = 2,    //主动关闭
    };

public:
    TSock(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket);
    ~TSock();

    bool connect(const std::string& ip, int32_t port);
    void start();

    void sendMsg(CMessage msg);
    boost::asio::ip::tcp::socket& getSocket();
    boost::asio::io_service& getIoService();
    void shutdown();
    void onClosed(CLOSED_TYPE type = CLOSED_TYPE::NORMAL);

    void setReceivedMsgCB(ReceivedMsgCallback cb);
private:
    bool syncConnect(const std::string& ip, int32_t port);
protected:
    TSockHdl getConnectionHdl();
    void closeSocket();
    void doWrite();
    void doRead();
    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(int seconds);
    void timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer);
protected:
    boost::asio::io_service&        m_io_service;
    boost::asio::ip::tcp::socket    m_socket;
    std::list<CMessage>             m_write_buffer;
    std::atomic<bool>               m_is_closed;
    ReceivedMsgCallback             m_received_msg_cb;
    ConnectionClosed                m_closed_cb;
    ConnectionTimeout               m_timeout_cb;
    int32_t                         m_read_timeout_seconds;
    std::vector<uint8_t>            m_read_buffer;
};

} // NLNET
