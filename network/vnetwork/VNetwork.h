#pragma once

#include <cstdlib>
#include <cstdint>
#include <deque>
#include <set>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <boost/asio.hpp>

namespace vnetwork
{

class ServerSession;
class ChatMessage;
class TcpServer;

typedef std::shared_ptr<ServerSession> ServerSessionPtr;
typedef std::shared_ptr<ChatMessage> ChatMessagePtr;

class ServerEvent
{
public:
    ServerEvent() {}
    virtual ~ServerEvent() {}

    virtual void OnAccepted(std::shared_ptr<ServerSession> session) = 0;
    virtual void OnClosed(std::shared_ptr<ServerSession> session) = 0;
    virtual void OnReceived(std::shared_ptr<ServerSession> session, std::shared_ptr<ChatMessage> msg) = 0;
};

class DefaultServerEvent : public ServerEvent
{
public:
    DefaultServerEvent() {}
    virtual ~DefaultServerEvent() {}

    virtual void OnAccepted(std::shared_ptr<ServerSession>) override {}
    virtual void OnClosed(std::shared_ptr<ServerSession>) override {}
    virtual void OnReceived(std::shared_ptr<ServerSession>, std::shared_ptr<ChatMessage>) override {}
};


class ClientEvent
{
public:
    ClientEvent() {}
    virtual ~ClientEvent() {}

    virtual void OnConnected(const boost::system::error_code& ec) = 0;
    virtual void OnClosed() = 0;
    virtual void OnReceived(std::shared_ptr<ChatMessage> msg) = 0;
};

class DefaultClientEvent : public ClientEvent
{
public:
    DefaultClientEvent() {}
    virtual ~DefaultClientEvent() {}

    virtual void OnConnected(const boost::system::error_code&) override {}
    virtual void OnClosed() override {}
    virtual void OnReceived(std::shared_ptr<ChatMessage>) override {}
};


class ChatMessage
{
public:
    enum { HEADER_LENGTH = 16 };
    enum { MAX_BODY_LENGTH = 512 };

    ChatMessage();
    ~ChatMessage();

    static void Create(const std::string& data, ChatMessage* out);
    static void Create(const void* data, std::size_t length, ChatMessage* out);

    const char* Data() const;
    char* Data();

    std::size_t Length() const;
    const char* Body() const;
    char* Body();
    std::size_t BodyLength() const;
    bool DecodeHeader();
    void EncodeHeader();

    void Reset();
    void PrepareBody(std::size_t len);

private:
    std::int32_t body_length_;
    std::vector<char> buffer_;
};


using boost::asio::ip::tcp;

typedef std::deque<ChatMessagePtr> chat_message_queue;

class TcpClient
{
public:
    TcpClient(boost::asio::io_service& io_service);
    ~TcpClient();

    void Connect(const std::string& ip, std::uint16_t port);

    void SetEvent(ClientEvent* evt);
    void AsyncSend(const std::string& msg);
    void AsyncSend(ChatMessagePtr msg);
    void AsyncClose();
private:
    void DoConnect(tcp::resolver::iterator endpoint_iterator);
    void DoReadHeader();
    void DoReadBody();
    void DoWrite();
    void Shutdown();

private:
    boost::asio::io_service&    io_service_;
    tcp::socket                 socket_;
    ChatMessagePtr              read_msg_;
    chat_message_queue          write_msgs_;
    DefaultClientEvent          default_event_;
    ClientEvent*                client_event_;
    bool                        closed_;
};

class ServerSession : public std::enable_shared_from_this<ServerSession>
{
public:
    ServerSession(tcp::socket socket, TcpServer& server, std::int64_t index);
    void Start();
    void AsyncSend(const std::string& msg);
    void AsyncSend(ChatMessagePtr msg);
    std::int64_t GetIndex() const;
    void AsyncClose();

private:
    void DoReadHeader();
    void DoReadBody();
    void DoWrite();
    void Shutdown(const ServerSessionPtr& session);

    tcp::socket         socket_;
    TcpServer&          server_;
    const std::int64_t  index_;
    ChatMessagePtr      read_msg_;
    chat_message_queue  write_msgs_;
    bool                closed_;
};


class TcpServer
{
public:
    TcpServer(boost::asio::io_service& io_service);
    ~TcpServer();

    bool Init(const std::string& ip, std::uint16_t port);
    void SetEvent(ServerEvent* evt);
    ServerEvent* GetServerEvent();
    boost::asio::io_service& GetIoService();

    void SessionJoin(ServerSessionPtr session);
    void SessionLeave(ServerSessionPtr session);

private:
    void DoAccept();
    std::int64_t NextIndex();
    bool Listen(const std::string& ip, std::uint16_t port);

    std::string                     ip_;
    std::uint16_t                   port_;
    boost::asio::io_service&        io_service_;
    std::shared_ptr<tcp::acceptor>  acceptor_;
    tcp::socket                     socket_;

    DefaultServerEvent              default_event_;
    ServerEvent*                    server_event_;
    std::atomic<std::int64_t>       index_generator_;
    std::unordered_map<std::int64_t, ServerSessionPtr> session_map_;
};


}  // namespace vnetwork

