#include "stdafx.h"
#include "vnetwork/VirtualServer.h"

namespace vnetwork
{

VirtualServerEvent::VirtualServerEvent()
{
}

VirtualServerEvent::~VirtualServerEvent()
{
}

void VirtualServerEvent::OnAccepted(std::shared_ptr<vnetwork::ServerSession> session)
{
    // TODO
    //DPrintf("server accept %d", (int)session->GetIndex());
}

void VirtualServerEvent::OnClosed(std::shared_ptr<vnetwork::ServerSession> session)
{
    //DPrintf("server closed %d", (int)session->GetIndex());
}

void VirtualServerEvent::OnReceived(std::shared_ptr<vnetwork::ServerSession> session, std::shared_ptr<vnetwork::ChatMessage> msg)
{
    /*
    int idx = (int)session->GetIndex();
    std::string s(msg->Body(), msg->Body() + msg->BodyLength());
    DPrintf("server received %d %s", idx, s.c_str());

    static int n = 0;
    std::string str = std::to_string(idx) + " replay: " + s + "  " + std::to_string(++n);
    session->AsyncSend(str);
    if (n >= 5) {
        session->AsyncClose();
    }
    */
}


VirtualServer::VirtualServer()
    : m_ios()
    , m_work()
    , m_thread()
    , m_server()
    , m_evt()
{
}

VirtualServer::~VirtualServer()
{
    Close();
    if (m_thread.joinable())
        m_thread.join();
}

bool VirtualServer::Init(std::string ip, std::uint16_t port)
{
    m_server = std::make_shared<vnetwork::TcpServer>(m_ios);
    m_server->SetEvent(&m_evt);
    if (!m_server->Init(ip, port)) {
        //WPrintf("init failed. %s %d", ip.c_str(), (int)port);
        return false;
    }

    m_work = std::make_shared<boost::asio::io_service::work>(m_ios);
    std::thread temp(&VirtualServer::StartThread, this);
    m_thread = std::move(temp);
    return true;
}

void VirtualServer::Close()
{
    m_work = nullptr;
    if (!m_ios.stopped())
        m_ios.stop();
}

void VirtualServer::StartThread()
{
    while (1) {
        try {
            m_ios.run();
            break;
        }
        catch (const std::exception& e) {
            //WPrintf("io_service exception: %s", e.what());
            m_ios.reset();
        }
    }
}

} // namespace vnetwork

