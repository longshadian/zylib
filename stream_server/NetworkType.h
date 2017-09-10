#pragma once

#include <memory>

namespace network {

class RWHandler;
using RWHandlerPtr = std::shared_ptr<RWHandler>;
using ConnectionHdl = std::weak_ptr<RWHandler>;

class Message;
using MessagePtr = std::shared_ptr<Message>;

}
