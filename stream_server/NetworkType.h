#pragma once

#include <memory>
#include <chrono>

namespace network {

class RWHandler;
using RWHandlerPtr = std::shared_ptr<RWHandler>;
using ConnectionHdl = std::weak_ptr<RWHandler>;
using ConnectionHdlLess = std::owner_less<ConnectionHdl>;

class Message;
using MessagePtr = std::shared_ptr<Message>;

class MessageContext;
using MessageContextUPtr = std::unique_ptr<MessageContext>;

using DiffTime = std::chrono::milliseconds;

}
