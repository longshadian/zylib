#pragma once

#include <functional>
#include <memory>

#include <boost/asio.hpp>

#include <net/InetAddress.h>
#include <net/Types.h>
#include <net/Utility.h>
#include <net/console_log.h>

namespace zysoft
{
namespace net
{

class EventLoop;

class Acceptor
{
public:
    using AcceptCallback = std::function<void(TcpConnectionPtr& conn, const boost::system::error_code& ec)>;
public:
    Acceptor(EventLoop* loop, InetAddress addr)
        : loop_(loop)
        , addr_(addr)
        , acceptor_(std::make_unique<boost::asio::ip::tcp::acceptor>(loop_->GetIOContext().get_executor()))
        , conn_()
        , accept_cb_()
        , reuse_addr_()
    {
    }

    ~Acceptor()
    {
    }

    Acceptor(const Acceptor&) = delete;
    Acceptor& operator=(const Acceptor&) = delete;
    Acceptor(Acceptor&&) = delete;
    Acceptor& operator=(Acceptor&&) = delete;

    void SetReuseAddr(bool b) { reuse_addr_ = b; }
    const InetAddress& GetAddr() const { return addr_; }

    bool Listen(std::string* err_msg = nullptr)
    {
        try {
            auto ep = util::CreateEndpoint(addr_.ip_, addr_.port_);
            acceptor_->open(ep.protocol());
            if (reuse_addr_)
                acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            acceptor_->bind(ep);
            acceptor_->listen();
            NET_LOG_TRACE << "init acceptor success listening " << addr_.IpPort_ToString();
            return true;
        } catch (const std::exception& e) {
            if (err_msg) {
                *err_msg = e.what();
            }
            //NET_LOG_CRIT << "init acceptor error. " << addr_.IpPort_ToString() << " reason: " << e.what();
            return false;
        }
    }

    void WaitAccept(TcpConnectionPtr conn, AcceptCallback cb)
    {
        conn_ = conn;
        accept_cb_ = std::move(cb);
        acceptor_->async_accept(conn_->GetSocket(),
            [this](const boost::system::error_code& ec) 
            {
                accept_cb_(conn_, ec);
            }
        );
    }

private:
    EventLoop* loop_;
    InetAddress addr_;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    TcpConnectionPtr conn_;
    AcceptCallback accept_cb_;
    bool reuse_addr_;
};

} // namespace net
} // namespace zysoft

