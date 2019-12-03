#include "stdafx.h"
#include "VNetwork.h"


namespace vnetwork
{

ChatMessage::ChatMessage()
    : body_length_(),
    buffer_()
{
    buffer_.resize(HEADER_LENGTH);
}

ChatMessage::~ChatMessage()
{

}

void ChatMessage::Create(const std::string& data, ChatMessage* out)
{
    out->body_length_ = static_cast<std::int32_t>(data.length());
    out->PrepareBody(out->body_length_);
    std::memcpy(out->Body(), data.data(), out->body_length_);
    out->EncodeHeader();
}

void ChatMessage::Create(const void* data, std::size_t length, ChatMessage* out)
{
    out->body_length_ = static_cast<std::int32_t>(length);
    out->PrepareBody(out->body_length_);
    std::memcpy(out->Body(), data, out->body_length_);
    out->EncodeHeader();
}

const char* ChatMessage::Data() const
{
    return buffer_.data();
}

char* ChatMessage::Data()
{
    return buffer_.data();
}

std::size_t ChatMessage::Length() const
{
    return HEADER_LENGTH + body_length_;
}

const char* ChatMessage::Body() const
{
    return buffer_.data() + HEADER_LENGTH;
}

char* ChatMessage::Body()
{
    return buffer_.data() + HEADER_LENGTH;
}

std::size_t ChatMessage::BodyLength() const
{
    return static_cast<std::size_t>(body_length_);
}

bool ChatMessage::DecodeHeader()
{
    std::int32_t body_len = 0;
    std::memcpy(&body_len, Data(), 4);
    if (body_len < 0)
        return false;

    if (MAX_BODY_LENGTH > 0) {
        if (body_len > MAX_BODY_LENGTH)
            return false;
    }

    body_length_ = body_len;
    PrepareBody(body_length_);
    return true;
}

void ChatMessage::EncodeHeader()
{
    std::array<char, HEADER_LENGTH> head_buffer{};
    head_buffer.fill(0);
    std::memcpy(head_buffer.data(), &body_length_, sizeof(body_length_));
    std::copy(head_buffer.begin(), head_buffer.end(), Data());
}

void ChatMessage::Reset()
{
    buffer_.clear();
    buffer_.resize(HEADER_LENGTH, '\0');
}

void ChatMessage::PrepareBody(std::size_t len)
{
    std::array<char, HEADER_LENGTH> head_bk{};
    std::copy(buffer_.begin(), buffer_.begin() + HEADER_LENGTH, head_bk.begin());

    buffer_.clear();
    buffer_.resize(HEADER_LENGTH + len, '\0');
    std::copy(head_bk.begin(), head_bk.end(), buffer_.begin());
}


TcpClient::TcpClient(boost::asio::io_service& io_service)
    : io_service_(io_service),
    socket_(io_service),
    read_msg_(std::make_shared<ChatMessage>()),
    write_msgs_(),
    default_event_(),
    client_event_(&default_event_),
    closed_()
{
    //DoConnect(endpoint_iterator);
}

TcpClient::~TcpClient()
{

}

void TcpClient::Connect(const std::string& ip, std::uint16_t port)
{
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
    socket_.async_connect(endpoint,
        [this](boost::system::error_code ec)
    {
        client_event_->OnConnected(ec);
        if (!ec)
        {
            DoReadHeader();
        }
    });
}

void TcpClient::SetEvent(ClientEvent* evt)
{
    if (!evt)
        return;
    client_event_ = evt;
}

void TcpClient::AsyncSend(const std::string& msg)
{
    auto cm = std::make_shared<ChatMessage>();
    ChatMessage::Create(msg, cm.get());
    AsyncSend(cm);
}

void TcpClient::AsyncSend(ChatMessagePtr msg)
{
    io_service_.post(
        [this, msg]()
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            DoWrite();
        }
    });
}

void TcpClient::AsyncClose()
{
    io_service_.post(
        [this]() 
    { 
        Shutdown();
    });
}

void TcpClient::DoConnect(tcp::resolver::iterator endpoint_iterator)
{
    boost::asio::async_connect(socket_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
    {
        if (!ec)
        {
            DoReadHeader();
        }
    });
}

void TcpClient::DoReadHeader()
{
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_->Data(), ChatMessage::HEADER_LENGTH),
        [this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec && read_msg_->DecodeHeader())
        {
            if (read_msg_->BodyLength() == 0) {
                client_event_->OnReceived(read_msg_);
                DoReadHeader();
            } else {
                DoReadBody();
            }
        } else {
            Shutdown();
        }
    });
}

void TcpClient::DoReadBody()
{
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_->Body(), read_msg_->BodyLength()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec) {
            client_event_->OnReceived(read_msg_);
            DoReadHeader();
        } else {
            Shutdown();
        }
    });
}

void TcpClient::DoWrite()
{
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()->Data(),
        write_msgs_.front()->Length()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                DoWrite();
            }
        } else {
            Shutdown();
        }
    });
}

void TcpClient::Shutdown()
{
    if (closed_) {
        return;
    }
    closed_ = true;
    boost::system::error_code ec{};
    socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
    socket_.close(ec);
    client_event_->OnClosed();
}

ServerSession::ServerSession(tcp::socket socket, TcpServer& server, std::int64_t index)
    : socket_(std::move(socket)),
    server_(server),
    index_(index),
    read_msg_(std::make_shared<ChatMessage>()),
    write_msgs_(),
    closed_()
{
}

void ServerSession::Start()
{
    DoReadHeader();
}

void ServerSession::AsyncSend(const std::string& msg)
{
    auto cm = std::make_shared<ChatMessage>();
    ChatMessage::Create(msg, cm.get());
    AsyncSend(cm);
}

void ServerSession::AsyncSend(ChatMessagePtr msg)
{
    auto& io_ctx = server_.GetIoService();
    auto self(shared_from_this());
    io_ctx.post([this, self, msg]()
    {
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            DoWrite();
        }
    });
}

std::int64_t ServerSession::GetIndex() const
{
    return index_;
}

void ServerSession::AsyncClose()
{
    auto self(shared_from_this());
    server_.GetIoService().post(
        [this, self]() 
    { 
        Shutdown(self);
    });
}

void ServerSession::DoReadHeader()
{
    read_msg_->Reset();
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_->Data(), ChatMessage::HEADER_LENGTH),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec && read_msg_->DecodeHeader())
        {
            if (read_msg_->BodyLength() == 0) {
                server_.GetServerEvent()->OnReceived(self, read_msg_);
                DoReadHeader();
            } else {
                DoReadBody();
            }
        } else {
            Shutdown(self);
        }
    });
}

void ServerSession::DoReadBody()
{
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_->Body(), read_msg_->BodyLength()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            server_.GetServerEvent()->OnReceived(self, read_msg_);
            DoReadHeader();
        } else {
            Shutdown(self);
        }
    });
}

void ServerSession::DoWrite()
{
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()->Data(),
        write_msgs_.front()->Length()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
        if (!ec)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                DoWrite();
            }
        } else {
            Shutdown(self);
        }
    });
}

void ServerSession::Shutdown(const ServerSessionPtr& self)
{
    if (closed_) {
        return;
    }
    closed_ = true;
    boost::system::error_code ec{};
    socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
    socket_.close(ec);
    server_.GetServerEvent()->OnClosed(self);
    server_.SessionLeave(self);
}

//----------------------------------------------------------------------

TcpServer::TcpServer(boost::asio::io_service& io_service)
    : ip_(),
    port_(),
    io_service_(io_service),
    acceptor_(),
    socket_(io_service),
    default_event_(),
    server_event_(&default_event_),
    index_generator_(),
    session_map_()
{
}

TcpServer::~TcpServer()
{
}

bool TcpServer::Init(const std::string& ip, std::uint16_t port)
{
    if (!Listen(ip, port))
        return false;
    DoAccept();
    return true;
}

void TcpServer::SetEvent(ServerEvent* evt)
{
    if (evt) {
        server_event_ = evt;
    }
}

ServerEvent* TcpServer::GetServerEvent()
{
    return server_event_;
}

boost::asio::io_service& TcpServer::GetIoService()
{
    return io_service_;
}

void TcpServer::DoAccept()
{
    acceptor_->async_accept(socket_,
        [this](boost::system::error_code ec)
    {
        if (!ec)
        {
            auto idx = NextIndex();
            auto p_session = std::make_shared<ServerSession>(std::move(socket_), *this, idx);
            p_session->Start();
            SessionJoin(p_session);
            GetServerEvent()->OnAccepted(p_session);
        }
        DoAccept();
    });
}

std::int64_t TcpServer::NextIndex()
{
    return ++index_generator_;
}

bool TcpServer::Listen(const std::string& ip, std::uint16_t port) 
{
    ip_ = ip;
    port_ = port;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ip), port);
    acceptor_ = std::make_shared<boost::asio::ip::tcp::acceptor>(io_service_);
    try
    {
        acceptor_->open(endpoint.protocol());
        acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_->bind(endpoint);
        acceptor_->listen();
        return true;
    } catch (const std::exception& e) {
        (void)e;
        return false;
    }
}

void TcpServer::SessionJoin(ServerSessionPtr session)
{
    session_map_[session->GetIndex()] = session;
}

void TcpServer::SessionLeave(ServerSessionPtr session)
{
    session_map_.erase(session->GetIndex());
}


} // namespace vnetwork


