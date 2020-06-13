#pragma once

#include <functional>

#include <net/ErrorCode.h>

namespace zysoft
{
namespace net
{

class Buffer;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer*)>;
using WriteCompleteCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using ErrorCallback = std::function<void(const TcpConnectionPtr&, const ErrorCode& ecode)>;


struct Tribool
{
    explicit
    Tribool(bool b) 
        : state_( b ? 1 : 0)
    {
    }

    Tribool()
        : state_(-1)
    {
    }

    ~Tribool() = default;
    Tribool(const Tribool&) = default; 
    Tribool& operator=(const Tribool&) = default; 
    Tribool(Tribool&&) = default; 
    Tribool& operator=(Tribool&&) = default; 

    Tribool& operator=(bool b)
    {
        state_ = b ? 1 : 0;
        return *this;
    }

    bool True() const
    {
        return state_ == 1;
    }

    bool False() const
    {
        return state_ == 0;
    }

    bool Indeterminate() const
    {
        return state_ == -1;
    }

    int state_;
};

namespace detail 
{

using FrameworkErrorCallback = std::function<void(const TcpConnectionPtr&, const ErrorCode*, bool)>;

} // namespace detail

} // namespace net
} // namespace zysoft


