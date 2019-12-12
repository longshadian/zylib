#pragma once

#include <cstdint>
#include <cassert>
#include <memory>
#include <boost/asio.hpp>

namespace network
{

using TcpAcceptorPtr = std::shared_ptr<boost::asio::ip::tcp::acceptor>;

enum class ECloseType : int
{
    Active        = 0,    // 主动关闭
    Read          = 1,    // 读关闭
    Write         = 2,    // 写关闭
    ReadTimeout   = 3,    // 读超时
    WriteTimeout  = 4,    // 写超时
};

} // namepsace network

#define NETWORK_ASSERT(stmt) assert(stmt)

