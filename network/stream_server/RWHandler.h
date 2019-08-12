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

struct HandlerOption
{
    size_t m_read_timeout_seconds;
};

class RWHandler : public std::enable_shared_from_this<RWHandler>
{
    using DeadlineTimerPtr = std::shared_ptr<boost::asio::deadline_timer>;

public:
    enum class CLOSED_TYPE : int
    {
        NORMAL   = 0,    //正常关闭
        TIMEOUT  = 1,    //超时关闭
        ACTIVITY = 2,    //主动关闭
    };

public:
    RWHandler(boost::asio::ip::tcp::socket socket, const HandlerOption& opt);
    ~RWHandler();
    RWHandler(const RWHandler&) = delete;
    RWHandler& operator=(const RWHandler&) = delete;
    RWHandler(RWHandler&&) = delete;
    RWHandler& operator=(RWHandler&&) = delete;

    void init();
    void sendMessage(MessagePtr msg);
    boost::asio::ip::tcp::socket& getSocket();
    boost::asio::io_service& getIOService();
    void shutdown();
    Hdl getHdl();

    void setCB_AsyncReceiveMsg(CB_ReceivedMessage cb);
    void setCB_AsyncTimeout(CB_HandlerTimeout cb);
    void setCB_AsyncClosed(CB_HandlerClosed cb);
    void setCB_AsyncDecode(CB_MessageDecoder cb);

private:
    bool asyncMsgDecode(std::vector<MessagePtr>* out);

    void doClosed(CLOSED_TYPE type);
    void doWrite();
    void doWriteCallback(boost::system::error_code ec, std::size_t length);
    void doRead();
    void doReadCallback(DeadlineTimerPtr timer, boost::system::error_code ec, std::size_t length);
    DeadlineTimerPtr setTimeoutTimer(size_t seconds);
    void timeoutCancel(DeadlineTimerPtr timer);

private:
    boost::asio::ip::tcp::socket    m_socket;
    HandlerOption                   m_handler_opt;
    std::atomic<bool>               m_is_closed;

    std::list<MessagePtr>           m_write_buffer;
    std::array<uint8_t, 1024>       m_read_fix_buffer;
    ByteBuffer                      m_read_buffer;

    CB_HandlerClosed                m_async_closed;
    CB_HandlerTimeout               m_async_timeout;
    CB_ReceivedMessage              m_async_receive_mgs;
    CB_MessageDecoder               m_async_decode;
};

}
