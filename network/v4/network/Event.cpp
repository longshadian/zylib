#include "network/Event.h"

#include "network/TcpServer.h"
#include "network/Utilities.h"

namespace network
{

void NetworkEvent::OnConnect(const boost::system::error_code& ec, TcpConnector& connector)
{
    (void)ec;
    (void)connector;
}

void NetworkEvent::OnAccept(const boost::system::error_code& ec, TcpServer& server, Channel& channel)
{
    (void)ec;
    (void)server;
    (void)channel;
}

void NetworkEvent::OnClosed(Channel& channel, ECloseType type)
{
    (void)channel;
    (void)type;
}

void NetworkEvent::OnRead(const boost::system::error_code& ec, std::size_t length, Channel& channel)
{
    (void)ec;
    (void)length;
    (void)channel;
}

void NetworkEvent::OnWrite(const boost::system::error_code& ec, std::size_t length, Channel& channel, const Message& msg)
{
    (void)ec;
    (void)length;
    (void)channel;
    (void)msg;
}

void NetworkEvent::OnReceivedMessage(Channel& channel, std::vector<Message> msg_vec)
{
    (void)channel;
    (void)msg_vec;
}

void NetworkEvent::OnAcceptOverflow()
{
}

/**
 * class NetworkEventFactory
 ************************************************************************/
NetworkFactory::NetworkFactory()
{
}

NetworkFactory::~NetworkFactory()
{
}

std::shared_ptr<NetworkEvent> NetworkFactory::CreateNetworkEvent()
{
    return nullptr;
}

std::shared_ptr<MessageDecoder> NetworkFactory::CreateMessageDecoder()
{
    return nullptr;
}

} // namespace network

