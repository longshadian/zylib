#pragma once

#include "vnetwork/VNetwork.h"

namespace vnetwork
{

class VirtualServerEvent : public vnetwork::ServerEvent
{
public:
    VirtualServerEvent();
    virtual ~VirtualServerEvent();

    virtual void OnAccepted(std::shared_ptr<vnetwork::ServerSession> session) override;
    virtual void OnClosed(std::shared_ptr<vnetwork::ServerSession> session) override;
    virtual void OnReceived(std::shared_ptr<vnetwork::ServerSession> session, std::shared_ptr<vnetwork::ChatMessage> msg) override;
};


class VirtualServer
{
public:
    VirtualServer();
    ~VirtualServer();
    bool Init(std::string ip, std::uint16_t port);
    void Close();

private:
    void StartThread();

    boost::asio::io_service                             m_ios;
    std::shared_ptr<boost::asio::io_service::work>      m_work;
    std::thread                                         m_thread;
    std::shared_ptr<vnetwork::TcpServer>                m_server;
    VirtualServerEvent                                  m_evt;
};


} // namespace vnetwork
