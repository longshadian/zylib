#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <list>

#include <google/protobuf/message.h>
#include <boost/asio.hpp>

#include "NetworkType.h"

class RWHandler : public std::enable_shared_from_this<RWHandler>
{
    enum class CLOSED_TYPE : int
    {
        NORMAL_CLOSED   = 0,    //正常关闭
        TIMEOUT_CLOSED  = 1,    //超时关闭
        ACTIVITY_CLOSED = 2,    //主动关闭
    };

    static const int TIMEOUT_SECONDS = 0;   //never timeout
public:
    RWHandler(boost::asio::io_service& io_service, boost::asio::ip::tcp::socket socket, int timeout = TIMEOUT_SECONDS);
    ~RWHandler() = default;

    void start();
    void sendMsg(int32_t msg_id, const ::google::protobuf::Message* msg);
    boost::asio::ip::tcp::socket& getSocket();
    void shutdownSocket();

    void setCallbackOnClosed(std::function<void(const RWHandlerPtr& conn)> f);
private:
    void onAccept();
    void onReceivedMsg(MessagePtr msg);
    void onClosed(CLOSED_TYPE type = CLOSED_TYPE::NORMAL_CLOSED);
    void closeSocket();

    void appendMsg(std::vector<uint8_t> msg);

    void doWrite();

    void doReadHead();
    void doReadBody();

    std::shared_ptr<boost::asio::deadline_timer> setTimeoutTimer(int seconds);
    void timeoutCancel(std::shared_ptr<boost::asio::deadline_timer> timer);
private:
    boost::asio::io_service&            m_io_service;
    boost::asio::ip::tcp::socket        m_socket;
    std::function<void(const RWHandlerPtr& conn)>         m_callback_on_closed;
    std::list<std::vector<uint8_t>>     m_write_buffer;

    std::array<uint8_t, 8>              m_read_head;
    std::vector<uint8_t>                m_read_body;
    int32_t                             m_head_length;
    uint32_t                            m_head_key;

    int                                 m_timeout;
    std::atomic<bool>                   m_is_closed;
};
