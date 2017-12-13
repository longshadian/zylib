#pragma once

#include <chrono>
#include <memory>
#include <functional>
#include <unordered_map>

namespace knet {

using DiffTime = std::chrono::milliseconds;

class ReceivedMsg;
class ReceivedMsgCtx;

using ReceivedMsgPtr = std::shared_ptr<ReceivedMsg>;
using ReceivedMsgCtxPtr = std::shared_ptr<ReceivedMsgCtx>;

using Duration = std::chrono::milliseconds;

using Callback = std::function<void()>;

class Timer;
using TimerPtr = std::shared_ptr<Timer>;
using TimerHdl = std::shared_ptr<Timer>;

struct EventTimer;
using EventTimerPtr = std::shared_ptr<EventTimer>;

class RPCContext;
using RPCContextUPtr = std::unique_ptr<RPCContext>;

using MsgType = std::string;
using MsgID   = int32_t;

struct ConsumerConf
{
    std::string m_group_id{};
    std::string m_broker_list{};
    std::string m_topic{};
    int32_t     m_partition{};
};
using ConsumerConfUPtr = std::unique_ptr<ConsumerConf>;

struct ProducerConf
{
    std::string m_broker_list{};
};
using ProducerConfUPtr = std::unique_ptr<ProducerConf>;

using Key = std::string;

using ServiceID = std::string;

using MsgCallback       = std::function<void(ReceivedMsgCtxPtr, ReceivedMsgPtr)>;
using MsgCallbackArray  = std::unordered_map<int32_t, MsgCallback>;

using RPCSuccessCB = std::function<void(ReceivedMsgPtr)>;
using RPCTimeoutCB = std::function<void()>;

} // knet
