#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <boost/system/system_error.hpp>
#include "network/Define.h"

namespace network
{

class TcpServer;
class TcpClient;
class Channel;
class TcpConnector;
class Message;
class MessageDecoder;

class NetworkEvent
{
public:
    NetworkEvent() = default;
    virtual ~NetworkEvent() = default;
    NetworkEvent(const NetworkEvent& rhs) = delete;
    NetworkEvent& operator=(const NetworkEvent& rhs) = delete;
    NetworkEvent(NetworkEvent&& rhs) = delete;
    NetworkEvent& operator=(NetworkEvent&& rhs) = delete;

    virtual void OnConnect(const boost::system::error_code& ec, TcpConnector& connector)
    {
        (void)ec;
        (void)connector;
    }

    virtual void OnAccept(const boost::system::error_code& ec, TcpServer& server, Channel& channel)
    {
        (void)ec;
        (void)server;
        (void)channel;
    }

    virtual void OnClosed(Channel& channel, ECloseType type)
    {
        (void)channel;
        (void)type;
    }

    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, Channel& channel)
    {
        (void)ec;
        (void)length;
        (void)channel;
    }

    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, Channel& channel, const Message& msg)
    {
        (void)ec;
        (void)length;
        (void)channel;
        (void)msg;
    }

    virtual void OnReceivedMessage(Channel& channel, std::vector<Message> msg_vec)
    {
        (void)channel;
        (void)msg_vec;
    }

    virtual void OnAcceptOverflow()
    {
    }

};

using NetworkEventPtr = std::shared_ptr<NetworkEvent>;

class NetworkFactory
{
public:
    NetworkFactory()
    {
    }

    virtual ~NetworkFactory()
    {
    }

    virtual std::shared_ptr<NetworkEvent>   CreateNetworkEvent()
    {
        return nullptr;
    }

    virtual std::shared_ptr<MessageDecoder> CreateMessageDecoder()
    {
        return nullptr;
    }
};

using NetworkFactoryPtr = std::shared_ptr<NetworkFactory>;

} // namespace network
