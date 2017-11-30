#pragma once

#include <chrono>
#include <memory>
#include <functional>

namespace knet {

using DiffTime = std::chrono::milliseconds;

class ReceivedMessage;
class KMessageContext;

using ReceivedMessagePtr = std::shared_ptr<ReceivedMessage>;
using MessageContextPtr = std::shared_ptr<KMessageContext>;

using ServiceID = std::string;
using RPCKey = uint64_t;

using Duration = std::chrono::milliseconds;

using Callback = std::function<void()>;

class TimerContext;
using TimerContextPtr = std::shared_ptr<TimerContext>;
using TimerHdl = std::shared_ptr<TimerContext>;

struct EventTimer;
using EventTimerPtr = std::shared_ptr<EventTimer>;

class RPCContext;
using RPCContextUPtr = std::unique_ptr<RPCContext>;

using MsgType = std::string;
using MsgID   = int32_t;

} // knet
