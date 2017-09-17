#pragma once

#include <memory>
#include <chrono>

#include <boost/system/system_error.hpp>

namespace network {

class ByteBuffer;
class StreamClient;

class RWHandler;
using RWHandlerPtr = std::shared_ptr<RWHandler>;
using ConnectionHdl = std::weak_ptr<RWHandler>;
using ConnectionHdlLess = std::owner_less<ConnectionHdl>;

class Message;
using MessagePtr = std::shared_ptr<Message>;

class MessageContext;
using MessageContextUPtr = std::unique_ptr<MessageContext>;

using DiffTime = std::chrono::milliseconds;

using CBHandlerClosed = std::function<void(ConnectionHdl)>;
using CBHandlerTimeout = std::function<void(ConnectionHdl)>;
using CBMessageDecode = std::function<void(ConnectionHdl, ByteBuffer& buffer, std::vector<MessagePtr>*)>;
using CBReceivedMessage = std::function<void(ConnectionHdl, std::vector<MessagePtr>)>;

using CBAcceptOverflow = std::function<void()>;
using CBAccept = std::function<void(ConnectionHdl)>;

using CBConnect = std::function<void(boost::system::error_code, StreamClient&)>;

}
