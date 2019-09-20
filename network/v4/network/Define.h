#pragma once

#include <cstdint>
#include <cassert>
#include <memory>
#include <boost/asio.hpp>

namespace network
{

using TcpSocketPtr = std::shared_ptr<boost::asio::ip::tcp::socket>;
using TcpAcceptorPtr = std::shared_ptr<boost::asio::ip::tcp::acceptor>;

} // namepsace network

#define NETWORK_ASSERT(stmt) assert(stmt)

