#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <boost/system/system_error.hpp>
#include "DllDefine.h"

class TcpServer;
class TcpClient;
class Channel;
class Message;

class NetworkEvent
{
public:
    NetworkEvent() = default;
    virtual ~NetworkEvent() = default;
    NetworkEvent(const NetworkEvent& rhs) = delete;
    NetworkEvent& operator=(const NetworkEvent& rhs) = delete;
    NetworkEvent(NetworkEvent&& rhs) = delete;
    NetworkEvent& operator=(NetworkEvent&& rhs) = delete;

    // 新的handler创建了
    virtual void OnConnect(const boost::system::error_code& ec, Channel& channel);

    // 新的handler创建了
    virtual void OnAccept(const boost::system::error_code& ec, TcpServer& server, Channel& channel);

    // handler关闭
    virtual void OnClosed(Channel& channel);

    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, Channel& channel);

    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, Channel& channel, const Message& msg);

    virtual void OnReceivedMessage(Channel& channel, std::vector<Message> msg_list);

    // handler超时
    virtual void OnTimeout(Channel& channel);

    // server可以得accept的handler超出上限
    virtual void OnAcceptOverflow();
};

using NetworkEventPtr = std::shared_ptr<NetworkEvent>;


class DllEvent : public NetworkEvent
{
public:
    DllEvent();
    virtual ~DllEvent() = default;

    virtual void OnConnect(const boost::system::error_code& ec, Channel& channel) override;
    virtual void OnAccept(const boost::system::error_code& ec, TcpServer& server, Channel& channel) override;
    virtual void OnClosed(Channel& channel) override;
    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, Channel& channel) override;
    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, Channel& channel, const Message& msg) override;
    virtual void OnReceivedMessage(Channel& channel, std::vector<Message> msg_list) override;
    virtual void OnTimeout(Channel& channel) override;
    virtual void OnAcceptOverflow() override;

    AcceptFunc        m_accect_func;
    ConnectFunc       m_connect_func;
    SendFunc          m_send_func;
    ReceiveFunc       m_receive_func;
    NetworkErrorFunc  m_network_error_func;
};

class TcpServerEvent : public DllEvent
{
public:
    TcpServerEvent();
    virtual ~TcpServerEvent();

    virtual void OnClosed(Channel& channel) override;

    TcpServer* m_server;
};

class TcpClientEvent : public DllEvent
{
public:
    TcpClientEvent();
    virtual ~TcpClientEvent();

    virtual void OnClosed(Channel& channel) override;

    TcpClient* m_client;
};


