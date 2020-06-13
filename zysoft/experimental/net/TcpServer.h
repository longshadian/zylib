#pragma once

#include <memory>
#include <unordered_map>

#include <boost/asio.hpp>

#include <net/Types.h>
#include <net/Defines.h>
#include <net/Acceptor.h>
#include <net/EventLoop.h>
#include <net/EventLoopThreadPool.h>
#include <net/InetAddress.h>
#include <net/Utility.h>

namespace zysoft
{
namespace net
{

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer
{
public:
    explicit
    TcpServer(EventLoop* loop, const InetAddress& addr)
        : loop_(loop)
        , acceptor_(std::make_unique<Acceptor>(loop_, addr))
        , thread_pool_(std::make_unique<EventLoopThreadPool>(loop_))
        , connection_cb_(std::bind(&util::DefaultConnectionCB, std::placeholders::_1))
        , message_cb_(std::bind(&util::DefaultMessageCB, std::placeholders::_1, std::placeholders::_2))
        , write_complete_cb_(std::bind(&util::DefaultWriteCompleteCB, std::placeholders::_1))
        , close_cb_(std::bind(&util::DefaultCloseCB, std::placeholders::_1))
        , error_cb_(std::bind(&util::DefaultErrorCB, std::placeholders::_1, std::placeholders::_2))
        , connections_()
        , nextConnId_(1)
    {
    }

    ~TcpServer() {}

    TcpServer(const TcpServer& rhs) = delete;
    TcpServer& operator=(const TcpServer& rhs) = delete;
    TcpServer(TcpServer&& rhs) = delete;
    TcpServer& operator=(TcpServer&& rhs) = delete;

    void SetConnectionCallback(ConnectionCallback cb) { connection_cb_ = std::move(cb); }
    void SetMessageCallback(MessageCallback cb) { message_cb_ = std::move(cb); }
    void SetWriteCompleteCallback(WriteCompleteCallback cb) { write_complete_cb_ = std::move(cb); } 
    void SetReuseAddr(bool b) { acceptor_->SetReuseAddr(b); }

    bool Start()
    {
        if (!acceptor_->Listen()) {
            return false;
        }
        thread_pool_->Start();
        StartAccept();
        return true;
    }

private:
 
    void StartAccept()
    {
        EventLoop* io_loop = thread_pool_->NextEventLoop();
        std::string conn_name = util::ToString(acceptor_->GetAddr(), nextConnId_);
        TcpConnectionPtr conn = std::make_shared<TcpConnection>(io_loop, std::move(conn_name));
        conn->SetConnectionCallback(connection_cb_);
        conn->SetMessageCallback(message_cb_);
        conn->SetWriteCompleteCallback(write_complete_cb_);
        //conn->setCloseCallback(std::bind(&TcpServer::RunInConnectionLoop_OnClose, this, std::placeholders::_1));
        //conn->SetErrorCallBack(std::bind(&TcpServer::RunInConnectionLoop_OnError, this, std::placeholders::_1, std::placeholders::_2));
        conn->SetFrameworkCallback(std::bind(&TcpServer::RunInConnectionLoop_OnErrorEx, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        ++nextConnId_;
        acceptor_->WaitAccept(conn, std::bind(&TcpServer::NewConnection, this, std::placeholders::_1, std::placeholders::_2));
    }

    void NewConnection(TcpConnectionPtr conn, const boost::system::error_code& ec)
    {
        if (ec) {
            NET_LOG_WARN << "Acceptor error. ecode: " << ec.value();
            acceptor_->WaitAccept(nullptr, std::bind(&TcpServer::NewConnection, this, std::placeholders::_1, std::placeholders::_2));
            return;
        }
        conn->SetState(TcpConnection::EState::Connecting);
        connections_[conn->GetConnName()] = conn;
        conn->GetEventLoop()->RunInLoop(std::bind(&TcpConnection::ConnectEstablised, conn));
        StartAccept();
    }

    void RunInConnectionLoop_OnErrorEx(const TcpConnectionPtr& conn, const ErrorCode* ec, bool app_cb)
    {
        if (app_cb) {
            try {
                if (ec->value() == boost::asio::error::eof) {
                    close_cb_(conn);
                } else {
                    error_cb_(conn, *ec);
                }
            } catch (...) { 
            }
        }
        loop_->RunInLoop(std::bind(&TcpServer::RemoveConnection, this, conn));
    }

    void RunInConnectionLoop_OnClose(const TcpConnectionPtr& conn)
    {
        try {
            close_cb_(conn); 
        } catch (...) {
        }
        loop_->RunInLoop(std::bind(&TcpServer::RemoveConnection, this, conn));
    }

    void RunInConnectionLoop_OnError(const TcpConnectionPtr& conn, const ErrorCode& ecode)
    {
        try { 
            error_cb_(conn, ecode); 
        } catch (...) {
        }
        loop_->RunInLoop(std::bind(&TcpServer::RemoveConnection, this, conn));
    }

    void RemoveConnection(const TcpConnectionPtr& conn)
    {
        const std::string& name = conn->GetConnName();
        NET_LOG_TRACE << "TcpServer remove connection: " << name;
        std::size_t n = connections_.erase(name);
        assert(n == 1);

        conn->GetEventLoop()->RunInLoop(std::bind(&TcpConnection::ConnectDestroy, conn));
    }

private:
    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<EventLoopThreadPool> thread_pool_;

    ConnectionCallback      connection_cb_;
    MessageCallback         message_cb_;
    WriteCompleteCallback   write_complete_cb_;
    CloseCallback           close_cb_;
    ErrorCallback           error_cb_;

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;
    ConnectionMap           connections_;
    int                     nextConnId_;
};


} // namespace net
} // namespace zysoft


