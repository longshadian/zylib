#pragma once

#include <cstdint>
#include <cassert>
#include <memory>
#include <boost/asio.hpp>

namespace network
{

using TcpSocketPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;
using TcpAcceptorPtr = std::shared_ptr<boost::asio::ip::tcp::acceptor>;

enum class ECloseType : int
{
    Active              = 0,    // 主动关闭
    ByRead              = 1,    // 读关闭
    ByWrite             = 2,    // 写关闭
    ByReadTimeout       = 3,    // 读超时
    ByWriteTimeout      = 4,    // 写超时
};

} // namepsace network

#define NETWORK_ASSERT(stmt) assert(stmt)

