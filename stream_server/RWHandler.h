#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "NetworkType.h"
#include "ByteBuffer.h"

namespace network {

class RWHandler;
class AsyncServer;

struct ConnectionInfo
{
    size_t m_timeout_seconds;
    std::string m_ip;
    std::string m_port;
};

class RWHandler : public std::enable_shared_from_this<RWHandler>
{
public:
    using ConnectionAccept = std::function<void(ConnectionHdl, const ConnectionInfo)>;
    using ConnectionClosed = std::function<void(ConnectionHdl)>;
    using ConnectionTimeout = std::function<void(ConnectionHdl)>;

    enum class CLOSED_TYPE : int
    {
        NORMAL   = 0,    //正常关闭
        TIMEOUT  = 1,    //超时关闭
        ACTIVITY = 2,    //主动关闭
    };

public:
    RWHandler(AsyncServer& async_server, boost::asio::ip::tcp::socket socket);
    ~RWHandler();

    void start();

    void sendMessage(MessagePtr msg);
    boost::asio::ip::tcp::socket& getSocket();
    boost::asio::io_service& getIOService();
    void shutdown();

    ConnectionHdl getHdl();
protected:
    void cbClosed();
    void cbTimeout();
    bool cbMsgDecode(std::vector<MessagePtr>* out);
    void cbReceivedMsg(std::vector<MessagePtr> messages);

    void onClosed(CLOSED_TYPE type);
    void doWrite();
    void doRead();
    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(size_t seconds);
    void timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer);
protected:
    AsyncServer&                    m_server;
    boost::asio::io_service&        m_io_service;
    boost::asio::ip::tcp::socket    m_socket;
    std::list<MessagePtr>           m_write_buffer;
    std::atomic<bool>               m_is_closed;
    ConnectionInfo                  m_conn_info;                      
    std::array<uint8_t, 1024>       m_read_fix_buffer;
    ByteBuffer                      m_read_buffer;
};

}
