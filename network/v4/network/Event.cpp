#include "network/Event.h"

#include "network/TcpServer.h"
#include "network/Utilities.h"

void NetworkEvent::OnConnect(const boost::system::error_code& ec, Channel& channel)
{
}

void NetworkEvent::OnAccept(const boost::system::error_code& ec, TcpServer& server, Channel& channel)
{
}

void NetworkEvent::OnClosed(Channel& channel)
{
}

void NetworkEvent::OnRead(const boost::system::error_code& ec, std::size_t length, Channel& channel)
{
}

void NetworkEvent::OnWrite(const boost::system::error_code& ec, std::size_t length, Channel& channel, const Message& msg)
{
}

void NetworkEvent::OnReceivedMessage(Channel& channel, std::vector<Message> msg_list)
{
}

void NetworkEvent::OnTimeout(Channel& channel)
{
}

void NetworkEvent::OnAcceptOverflow()
{
}


/**
 * class DllEvent
 ****************************************************************/
/*
DllEvent::DllEvent()
    : NetworkEvent()
    , m_accect_func()
    , m_connect_func()
    , m_send_func()
    , m_receive_func()
    , m_network_error_func()
{
}

void DllEvent::OnConnect(const boost::system::error_code& ec, Channel& channel)
{
    if (ec) {
        if (!m_network_error_func)
            return;
        std::string str = ec.message();
        m_network_error_func(channel.MetaIndex(), channel.Index(), EShutdownType::ConnectError
            , ec.value(), str.c_str(), Utilities::NullString(), 0);
        return;
    }

    if (m_connect_func)
        m_connect_func(channel.MetaIndex());
}

void DllEvent::OnAccept(const boost::system::error_code& ec, TcpServer& server, Channel& channel)
{
    if (ec) {
        if (!m_network_error_func)
            return;
        std::string str = ec.message();
        m_network_error_func(channel.MetaIndex(), channel.Index(), EShutdownType::AcceptError
            , ec.value(), str.c_str(), Utilities::NullString(), 0);
        return;
    }
    if (m_accect_func)
        m_accect_func(channel.MetaIndex(), channel.Index());
}

void DllEvent::OnClosed(Channel& channel)
{
}

void DllEvent::OnRead(const boost::system::error_code& ec, std::size_t length, Channel& channel)
{
}

void DllEvent::OnWrite(const boost::system::error_code& ec, std::size_t length, Channel& channel, const Message& msg)
{
}

void DllEvent::OnReceivedMessage(Channel& channel, std::vector<Message> msg_list)
{
}

void DllEvent::OnTimeout(Channel& channel)
{
}

void DllEvent::OnAcceptOverflow()
{

}
*/

/**
 * class TcpServerEvent
 ****************************************************************/
/*
TcpServerEvent::TcpServerEvent()
    : DllEvent()
    , m_server()
{
}

TcpServerEvent::~TcpServerEvent()
{
}

void TcpServerEvent::OnClosed(Channel& channel)
{

}
*/

/**
 * class TcpClientEvent
 ****************************************************************/

/*
TcpClientEvent::TcpClientEvent()
    : DllEvent()
    , m_client()
{
}

TcpClientEvent::~TcpClientEvent()
{
}

void TcpClientEvent::OnClosed(Channel& channel)
{

}
*/

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
    // TODO
    return nullptr;
}

std::shared_ptr<MessageDecoder> NetworkFactory::CreateMessageDecoder()
{
    // TODO
    return nullptr;
}
