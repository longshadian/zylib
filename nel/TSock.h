#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "Types.h"
#include "ByteBuf.h"

namespace nlnet {

struct ConnectionInfo
{
    size_t m_timeout_seconds;
    std::string m_ip;
    std::string m_port;
};

class TSock : public std::enable_shared_from_this<TSock>
{
public:
    using Closed_Callback = std::function<void(TSockPtr)>;
    using Timeout_Callback = std::function<void(TSockPtr)>;
    using ReceivedMsg_Callback = std::function<void(NetworkMessagePtr)>;

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

    bool isClosed() const;
    bool sendMsg(CMessagePtr msg);
    boost::asio::ip::tcp::socket& getSocket();
    boost::asio::io_service& getIoService();
    void shutdown();
    void onClosed(CLOSED_TYPE type = CLOSED_TYPE::NORMAL);

    void setReceivedMsgCallback(ReceivedMsg_Callback cb);
    void setClosedCallback(Closed_Callback cb);
    void setTimeoutCallback(Timeout_Callback cb);
    void setMessageDecodeCallback(ByteToMessage_Callback cb);

    TSockHdl getSockHdl();
private:
    boost::asio::io_service& getIOService();

protected:
    TSockHdl getConnectionHdl();
    void closeSocket();
    void doWrite();
    void doRead();
    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(int seconds);
    void timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer);
protected:
    boost::asio::ip::tcp::socket    m_socket;
    std::list<CMessagePtr>          m_write_buffer;
    std::atomic<bool>               m_is_closed;
    ReceivedMsg_Callback             m_received_msg_cb;
    Closed_Callback                  m_closed_cb;
    Timeout_Callback                 m_timeout_cb;
    ByteToMessage_Callback          m_msg_decoder_cb;
    int32_t                         m_read_timeout_seconds;
    std::array<uint8_t, 1024*4>     m_read_buffer;
    ByteBuf                         m_byte_buf;
};

} // NLNET
