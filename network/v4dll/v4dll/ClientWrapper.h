#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>

#include "network/Network.h"

#include "DllDefine.h"

class TcpClientWrapper;

class TcpClientEvent : public network::NetworkEvent
{
public:
    TcpClientEvent() = default;
    virtual ~TcpClientEvent() = default;

    virtual void OnConnect(const boost::system::error_code& ec, network::TcpConnector& connector) override;
    virtual void OnAccept(const boost::system::error_code& ec, network::TcpServer& server, network::Channel& channel) override;
    virtual void OnClosed(network::Channel& channel, network::ECloseType type) override;
    virtual void OnRead(const boost::system::error_code& ec, std::size_t length, network::Channel& channel) override;
    virtual void OnWrite(const boost::system::error_code& ec, std::size_t length, network::Channel& channel, const network::Message& msg) override;
    virtual void OnReceivedMessage(network::Channel& channel, std::vector<network::Message> msg_list) override;
    virtual void OnAcceptOverflow() override;

    OnSendCallback      m_send_cb;
    OnReceiveCallback   m_recv_cb;
};

class TcpClientEventFactory : public network::NetworkFactory
{
public:
    TcpClientEventFactory(TcpClientWrapper* cw);
    virtual ~TcpClientEventFactory() = default;

    virtual std::shared_ptr<network::NetworkEvent>   CreateNetworkEvent() override;
    virtual std::shared_ptr<network::MessageDecoder> CreateMessageDecoder() override;

    TcpClientWrapper* m_client_wrapper;
};


class TcpConnectorWrapper
{
public:
    TcpConnectorWrapper();
    ~TcpConnectorWrapper();

    ActorID m_id;
    network::TcpConnectorPtr m_conn;
};
using TcpConnectorWrapperPtr = std::shared_ptr<TcpConnectorWrapper>;

class TcpClientWrapper
{
public:
    TcpClientWrapper();
    ~TcpClientWrapper();

    void Init();

    TcpConnectorWrapperPtr CreateConnector(ActorID id);
    TcpConnectorWrapperPtr FindConnectorByActorID(ActorID id);
    void RemoveByActorID(ActorID id);

    std::shared_ptr<network::TcpClient> m_client;

    OnSendCallback      m_send_cb;
    OnReceiveCallback   m_recv_cb;

    std::mutex          m_mtx;
    std::unordered_map<ActorID, TcpConnectorWrapperPtr> m_map;
};
