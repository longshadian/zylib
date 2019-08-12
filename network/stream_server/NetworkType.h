#pragma once

#include <memory>
#include <chrono>

#include <boost/system/system_error.hpp>

namespace network {

class ByteBuffer;

class StreamClient;
using StreamClientPtr = std::shared_ptr<StreamClient>;

class RWHandler;
using RWHandlerPtr = std::shared_ptr<RWHandler>;
using Hdl = std::weak_ptr<RWHandler>;
using HdlLess = std::owner_less<Hdl>;

class Message;
using MessagePtr = std::shared_ptr<Message>;

using DiffTime = std::chrono::milliseconds;


// handler关闭
using CB_HandlerClosed = std::function<void(Hdl)>;

// handler超时
using CB_HandlerTimeout = std::function<void(Hdl)>;

// handler解码
using CB_MessageDecoder = std::function<void(Hdl, ByteBuffer& buffer, std::vector<MessagePtr>*)>;

// handler收到消息
using CB_ReceivedMessage = std::function<void(Hdl, std::vector<MessagePtr>)>;



// server可以得accept的handler超出上限
using CB_AcceptOverflow = std::function<void()>;

// server accept handler
using CB_Accept = std::function<void(Hdl)>;



// client 链接了server
using CB_Connect = std::function<void(boost::system::error_code, StreamClient&)>;

}
