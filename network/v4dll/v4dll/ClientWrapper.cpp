#include "ClientWrapper.h"



void TcpClientEvent::OnConnect(const boost::system::error_code& ec, network::TcpConnector& connector)
{
    if (ec) {
    } else {
    }
}

void TcpClientEvent::OnAccept(const boost::system::error_code& ec, network::TcpServer& server, network::Channel& channel)
{
    if (ec) {
    } else {
    }
}

void TcpClientEvent::OnClosed(network::Channel& channel, network::ECloseType type)
{
}

void TcpClientEvent::OnRead(const boost::system::error_code& ec, std::size_t length, network::Channel& channel)
{
    if (ec) {
    } else {
    }
}

void TcpClientEvent::OnWrite(const boost::system::error_code& ec, std::size_t length, network::Channel& channel, const network::Message& msg)
{
    if (ec) {
    } else {
    }
}

void TcpClientEvent::OnReceivedMessage(network::Channel& channel, std::vector<network::Message> msg_list)
{
    for (const auto& msg : msg_list) {
        std::string s(msg.BodyPtr(), msg.BodyPtr() + msg.BodyLength());
    }
}

void TcpClientEvent::OnAcceptOverflow()
{
}


TcpClientEventFactory::TcpClientEventFactory(TcpClientWrapper* cw)
    : network::NetworkFactory(), 
    m_client_wrapper(cw)
{
}

std::shared_ptr<network::NetworkEvent> TcpClientEventFactory::CreateNetworkEvent()
{
    auto p = std::make_shared<TcpClientEvent>();
    p->m_send_cb = m_client_wrapper->m_send_cb;
    p->m_recv_cb = m_client_wrapper->m_recv_cb;
}

std::shared_ptr<network::MessageDecoder> TcpClientEventFactory::CreateMessageDecoder()
{
    return network::CreateDefaultMessageDecoder();
}




TcpClientWrapper::TcpClientWrapper()
{
}

TcpClientWrapper::~TcpClientWrapper()
{

}

void TcpClientWrapper::Init()
{
    auto fac = std::make_shared<TcpClientEventFactory>();
    auto p = std::make_shared<network::TcpClient>(fac);
    p->Start(1);
    m_client = p;
}

network::TcpConnectorPtr TcpClientWrapper::CreateConnector(ActorID id)
{
}

network::TcpConnectorPtr TcpClientWrapper::FindConnectorByActorID(ActorID id)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    auto it = m_map.find(id);
    if (it == m_map.end())
        return nullptr;
    return it->second;
}

void TcpClientWrapper::RemoveByActorID(ActorID id)
{
    std::lock_guard<std::mutex> lk{ m_mtx };
    m_map.erase(id);
}
