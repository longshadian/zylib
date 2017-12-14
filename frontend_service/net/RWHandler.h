#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>

#include <boost/asio.hpp>

#include "net/NetworkType.h"
#include "net/Message.h"

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

private:
    void doClosed(CLOSED_TYPE type);
    void doWrite();
    void doWriteCallback(boost::system::error_code ec, std::size_t length);
    void doReadHead();
    void doReadBody();
    void timeoutCancel(DeadlineTimerPtr timer);

    static DeadlineTimerPtr setTimeoutTimer(size_t seconds);

private:
    StreamServer&                   m_stream_server;
    boost::asio::ip::tcp::socket    m_socket;
    HandlerOption                   m_handler_opt;
    std::atomic<bool>               m_is_closed;

    std::list<MessagePtr>           m_write_buffer;
    std::array<uint8_t, MSG_HEAD_SIZE> m_read_head;
    std::vector<uint8_t>            m_read_body;
};
