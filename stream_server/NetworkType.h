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


// handler�ر�
using CB_HandlerClosed = std::function<void(Hdl)>;

// handler��ʱ
using CB_HandlerTimeout = std::function<void(Hdl)>;

// handler����
using CB_MessageDecoder = std::function<void(Hdl, ByteBuffer& buffer, std::vector<MessagePtr>*)>;

// handler�յ���Ϣ
using CB_ReceivedMessage = std::function<void(Hdl, std::vector<MessagePtr>)>;



// server���Ե�accept��handler��������
using CB_AcceptOverflow = std::function<void()>;

// server accept handler
using CB_Accept = std::function<void(Hdl)>;



// client ������server
using CB_Connect = std::function<void(boost::system::error_code, StreamClient&)>;

}
