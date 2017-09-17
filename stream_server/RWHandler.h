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

    void sendMessage(MessagePtr msg);
    boost::asio::ip::tcp::socket& getSocket();
    boost::asio::io_service& getIOService();
    void shutdown();

    ConnectionHdl getHdl();

    void setCBReceiveMsg(CBReceivedMessage cb);
    void setCBTimeout(CBHandlerTimeout cb);
    void setCBClosed(CBHandlerClosed cb);
    void setCBDecode(CBMessageDecode cb);
private:
    void cbClosed();
    void cbTimeout();
    bool cbMsgDecode(std::vector<MessagePtr>* out);
    void cbReceivedMsg(std::vector<MessagePtr> messages);

    void onClosed(CLOSED_TYPE type);
    void doWrite();
    void doRead();
    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(size_t seconds);
    void timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer);

private:
    boost::asio::ip::tcp::socket    m_socket;
    HandlerOption                   m_handler_opt;                      
    std::atomic<bool>               m_is_closed;

    std::list<MessagePtr>           m_write_buffer;
    std::array<uint8_t, 1024>       m_read_fix_buffer;
    ByteBuffer                      m_read_buffer;

    CBHandlerClosed     m_cb_closed;
    CBHandlerTimeout    m_cb_timeout;
    CBReceivedMessage   m_cb_receive_mgs;
    CBMessageDecode     m_cb_decode;
};

}
