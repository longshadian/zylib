#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "Types.h"

namespace NLNET {

class TSock;

struct ConnectionInfo
{
    size_t m_timeout_seconds;
    std::string m_ip;
    std::string m_port;
};

struct TSockContext
{
    TSockContext(const std::string& sname,
        const ServiceID& sid,
        TSockPtr sock)
        : m_service_name(sname)
        , m_service_id(sid)
        , m_sock(std::move(sock))
    {
    }

    const std::string&  m_service_name;
    const ServiceID&    m_service_id;
    TSockPtr            m_sock;
};

class TSock : public std::enable_shared_from_this<TSock>
{
public:
    using ClosedCallback = std::function<void(TSockPtr)>;
    using TimeoutCallback = std::function<void(TSockPtr)>;
    using ReceivedMsgCallback = std::function<void(NetWorkMessagePtr)>;

    enum class CLOSED_TYPE : int
    {
        NORMAL   = 0,    //正常关闭
        TIMEOUT  = 1,    //超时关闭
        ACTIVITY = 2,    //主动关闭
    };

public:
    TSock(boost::asio::ip::tcp::socket socket);
    ~TSock();

    void start();

    bool sendMsg(CMessage msg);
    boost::asio::ip::tcp::socket& getSocket();
    boost::asio::io_service& getIoService();
    void shutdown();
    void onClosed(CLOSED_TYPE type = CLOSED_TYPE::NORMAL);

    void setReceivedMsgCallback(ReceivedMsgCallback cb);
    void setClosedCallback(ClosedCallback cb);
    void setTimeoutCallback(TimeoutCallback cb);

    TSockHdl getSockHdl();
    //const std::string& getServiceName() const;
    //const ServiceID& getServiceID() const;
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
    boost::asio::ip::tcp::socket    m_socket;
    std::list<std::vector<uint8_t>> m_write_buffer;
    std::atomic<bool>               m_is_closed;
    ReceivedMsgCallback             m_received_msg_cb;
    ClosedCallback                  m_closed_cb;
    TimeoutCallback                 m_timeout_cb;
    int32_t                         m_read_timeout_seconds;
    std::array<uint8_t, 4>          m_read_head;
    std::vector<uint8_t>            m_read_buffer;

};

} // NLNET
