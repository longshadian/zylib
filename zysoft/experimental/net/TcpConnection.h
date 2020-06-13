#pragma once

#include <array>
#include <list>
#include <string>

#include <boost/asio.hpp>

#include <net/Buffer.h>
#include <net/Types.h>
#include <net/EventLoop.h>
#include <net/console_log.h>

namespace zysoft
{
namespace net
{

class EventLoop;
class Buffer;

class TcpConnection 
    : public std::enable_shared_from_this<TcpConnection>
{
public:
    using socket_type = boost::asio::ip::tcp::socket;

    static const std::size_t FixedBufferSize = 4 * 1024;
    enum class EState
    {
        Closed,
        Connecting,
        Established,
        //Closing,

        //FinWait,           // 调用
        CloseWait,
    };

    enum EFlag
    {
        OptShutdown                 = 0x0001,
        OptForeceClose              = 0x0002,

        OptDoNotMessageCallback     = 0x0004,
    };

public:
    TcpConnection(EventLoop* loop, std::string conn_name)
        : loop_(loop)
        , conn_name_(std::move(conn_name))
        , socket_(loop_->GetIOContext().get_executor())
        , state_(EState::Closed)
        , flag_()
        , input_fixed_buffer_()
        , input_buffer_()
        , output_buffer_()
        , handle_error_(false)
        , connection_cb_()
        , message_cb_()
        , write_complete_cb_()
        //, close_cb_()
        //, error_cb_()
        , framework_cb_()
    {
    }

    ~TcpConnection()
    {
        ConnectDestroy();
    }

    TcpConnection(const TcpConnection&) = delete;
    TcpConnection& operator=(const TcpConnection&) = delete;
    TcpConnection(TcpConnection&&) = delete;
    TcpConnection& operator=(TcpConnection&&) = delete;

    void SetConnectionCallback(ConnectionCallback cb) { connection_cb_ = std::move(cb); } 
    void SetMessageCallback(MessageCallback cb) { message_cb_ = std::move(cb); }
    void SetWriteCompleteCallback(WriteCompleteCallback cb) { write_complete_cb_ = std::move(cb); }
    //void setCloseCallback(CloseCallback cb) { close_cb_ = std::move(cb); }
    //void SetErrorCallBack(ErrorCallback cb) { error_cb_ = std::move(cb); }
    void SetFrameworkCallback(detail::FrameworkErrorCallback cb) { framework_cb_ = std::move(cb); }

    const std::string& GetConnName() const { return conn_name_; }
    EventLoop* GetEventLoop() { return loop_; }
    socket_type& GetSocket() { return socket_; }

    // TODO private
    void SetState(EState s)
    {
        state_ = s;
    }

    void ConnectEstablised()
    {
        state_ = EState::Established;
        AsyncRead();
        try {
            connection_cb_(shared_from_this());
        } catch (...) {
        }
    }

    void ConnectDestroy()
    {
        if (state_ == EState::Closed)
            return;
        state_ = EState::Closed;
        boost::system::error_code ec;
        socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
        ec.clear();
        socket_.close(ec);
    }

    void Send(const std::string& str) { Send(str.data(), str.size()); }
    void Send(const void* p, std::size_t len)
    {
        Buffer b;
        b.Append(p, len);
        Send(std::move(b));
    }
    void Send(const Buffer& message) { SendImpl(Buffer(message)); }
    void Send(Buffer&& message) { SendImpl(std::move(message)); }

    void Shutdown(bool callback = false)
    {
        if (flag_ & EFlag::OptShutdown)
            return;
        if (flag_ & EFlag::OptForeceClose)
            return;
        flag_ |= EFlag::OptShutdown;
        if (!callback)
            flag_ |= EFlag::OptDoNotMessageCallback;

#if 1
        // 不能直接执行socket cancel，需要异步执行，理由如下：
        // 当前可能是HandleRead事件回调，此时如果未注册Write事件，cancel不会触发HandelError
        loop_->RunInLoop( 
            [this]() mutable 
            { 
                ShutdownInLoop(); 
            } 
        );
#else
        if (loop_->IsInLoopThread()) {
            ShutdownInLoop();
        } else {
            loop_->RunInLoop( 
                [this]() mutable 

                { 
                    ShutdownInLoop(); 
                } 
            );
        }
#endif
    }

    void ForceClose() 
    {
        if (flag_ & EFlag::OptForeceClose)
            return;
        flag_ |= EFlag::OptForeceClose;
        flag_ |= EFlag::OptDoNotMessageCallback;

#if 1
        // 不能直接执行socket cancel，需要异步执行，理由如下：
        // 当前可能是HandleRead事件回调，此时如果未注册Write事件，cancel不会触发HandelError
        loop_->RunInLoop( 
            [this]() mutable 
            { 
                ForceCloseInLoop();
            } 
        );
#else
        if (loop_->IsInLoopThread()) {
            ForceCloseInLoop();
        } else {
            loop_->RunInLoop( 
                [this]() mutable 
                { 
                    ForceCloseInLoop();
                } 
            );
        }
#endif
    }

    void AssertRunInLoop()
    {
        assert(loop_->IsInLoopThread());
    }

private:
    void AsyncRead()
    {
        socket_.async_read_some(boost::asio::buffer(input_fixed_buffer_),
            std::bind(&TcpConnection::HandleRead, this, std::placeholders::_1, std::placeholders::_2));
    }

    void AsyncWrite()
    {
        const auto& buffer = output_buffer_.front();
        boost::asio::async_write(socket_, boost::asio::buffer(buffer.ReadablePtr(), buffer.ReadableBytes()),
            std::bind(&TcpConnection::HandleWrite, this, std::placeholders::_1, std::placeholders::_2));
    }

    void HandleRead(boost::system::error_code ec, std::size_t len)
    {
        if (ec) {
            HandleError(ec);
            return;
        }
        NET_LOG_TRACE << GetConnName() << " read bytes: " << len;
        input_buffer_.Append(input_fixed_buffer_.data(), len);
        if (NeedMessageCallback()) {
            try {
                message_cb_(shared_from_this(), &input_buffer_);
            } catch (...) {
            }
        } else {
            // 应用程序主动调用shutdown，此时socket可能还处在正常连接状态，
            // 会接收到对端发送的数据，直接丢弃，不执行应用程序回调。
            input_buffer_.Clear();
        }
        AsyncRead();
    }

    void HandleWrite(boost::system::error_code ec, std::size_t len)
    {
        Buffer msg = std::move(output_buffer_.front());
        (void)msg;
        if (ec) {
            HandleError(ec);
            return;
        }
        try {
            if (write_complete_cb_) {
                write_complete_cb_(shared_from_this());
            }
        } catch (...) {
        }
        output_buffer_.pop_front();

        if (state_ == EState::Closed)
            return;

        if (state_ == EState::CloseWait) {
            if (flag_ & EFlag::OptForeceClose)
                return;
            if (flag_ & EFlag::OptShutdown) {
                if (output_buffer_.empty()) {
                    // 当前无消息可发送，取消异步操作，HandleRead会被执行。
                    boost::system::error_code ec;
                    socket_.cancel(ec);
                } else {
                    // 还有消息待发送，继续发送。
                    AsyncWrite();
                }
            }
            return;
        }

        if (!output_buffer_.empty()) {
            AsyncWrite();
        }
    }

    void HandleError(const boost::system::error_code& ec)
    {
        // 此函数有2种情况会被调用:
        // 1.应用程序主动调用Shutdown或ForceClose，触发read/write取消操作，进入此函数。
        //     此时，socket状态已经处于Closed状态
        // 2.read/write操作返回错误，例如对端主动关闭socket，或者socket read，write出错，进入此函数。

        AssertRunInLoop();
        NET_LOG_TRACE << "Handle Close. " << GetConnName() << " error code: " << ec.value() << " reason: " << ec.message();
        if (state_ != EState::Closed && state_ != EState::CloseWait) {
            state_ = EState::CloseWait;
        }

        // 确保只此函数只会被调用一次。防止FrameworkCallback执行多次。
        if (handle_error_)
            return;
        handle_error_ = true;
        framework_cb_(shared_from_this(), &ec, NeedMessageCallback());
        /*
        if (ec.value() == boost::asio::error::operation_aborted) {
        }
        */
    }

    void SendImpl(Buffer&& message)
    {
        if (loop_->IsInLoopThread()) {
            SendInLoop(std::move(message));
        } else {
            loop_->RunInLoop(
                [this, msg = std::move(message)]() mutable 
                { 
                    SendInLoop(std::move(msg)); 
                }
            );
        }
    }

    void SendInLoop(Buffer&& msg)
    {
        if (state_ == EState::CloseWait || state_ == EState::Closed) {
            return;
        }
        bool wait_write = !output_buffer_.empty();
        output_buffer_.emplace_back(std::move(msg));
        if (!wait_write) {
            AsyncWrite();
        }
    }

    void ShutdownInLoop()
    {
        if (state_ == EState::Closed || state_ == EState::CloseWait) {
            return;
        }
        state_ = EState::CloseWait;
        if (output_buffer_.empty()) {
            // 当前无消息待发送，取消异步操作，HandleRead会被调用。
            boost::system::error_code ec;
            socket_.cancel(ec);
        }
    }

    void ForceCloseInLoop()
    {
        if (state_ == EState::Closed) {
            return;
        }
        if (state_ != EState::CloseWait) {
            state_ = EState::CloseWait;
        }
        // 取消异步操作，HandleRead，HandleWrite(如果存在)会被调用。
        boost::system::error_code ec;
        socket_.cancel(ec);
    }

    bool NeedMessageCallback() const
    {
        return ! (flag_ & EFlag::OptDoNotMessageCallback);
    }

private:
    EventLoop*                  loop_;
    std::string                 conn_name_;
    socket_type                 socket_;
    EState                      state_;
    int                         flag_;
    std::array<char, FixedBufferSize> input_fixed_buffer_;
    Buffer                      input_buffer_;
    std::list<Buffer>           output_buffer_;
    bool                        handle_error_;

    ConnectionCallback          connection_cb_;
    MessageCallback             message_cb_;
    WriteCompleteCallback       write_complete_cb_;
    //CloseCallback               close_cb_;
    //ErrorCallback               error_cb_;
    detail::FrameworkErrorCallback framework_cb_;
};

} // namespace net
} // namespace zysoft

