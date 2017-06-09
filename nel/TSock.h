#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "Types.h"

namespace NLNET {

class TSock;
class UnifiedConnection;

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
    using ReceivedMsgCallback = std::function<void(NetWorkMessagePtr, TSockHdl)>;

    enum class CLOSED_TYPE : int
    {
        NORMAL   = 0,    //正常关闭
        TIMEOUT  = 1,    //超时关闭
        ACTIVITY = 2,    //主动关闭
    };

public:
    TSock(boost::asio::ip::tcp::socket socket, UnifiedConnection& conn);
    ~TSock();

    void start();

    void sendMsg(CMessage msg);
    boost::asio::ip::tcp::socket& getSocket();
    boost::asio::io_service& getIoService();
    void shutdown();
    void onClosed(CLOSED_TYPE type = CLOSED_TYPE::NORMAL);

    void setReceivedMsgCB(ReceivedMsgCallback cb);

    TSockHdl getSockHdl();
private:
    boost::asio::io_service& getIOService();

protected:
    TSockHdl getConnectionHdl();
    void closeSocket();
    void doWrite();
    void doRead();
    void doReadBody();
    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(int seconds);
    void timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer);
protected:
    UnifiedConnection&              m_conn;
    boost::asio::ip::tcp::socket    m_socket;
    std::list<std::vector<uint8_t>> m_write_buffer;
    std::atomic<bool>               m_is_closed;
    ReceivedMsgCallback             m_received_msg_cb;
    ConnectionClosed                m_closed_cb;
    ConnectionTimeout               m_timeout_cb;
    int32_t                         m_read_timeout_seconds;
    std::array<uint8_t, 4>          m_read_head;
    std::vector<uint8_t>            m_read_buffer;
};

} // NLNET
