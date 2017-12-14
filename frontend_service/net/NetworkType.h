#pragma once

#include <memory>
#include <chrono>
#include <type_traits>

class RWHandler;
using RWHandlerPtr = std::shared_ptr<RWHandler>;
using Hdl = std::weak_ptr<RWHandler>;
using HdlLess = std::owner_less<Hdl>;

class Message;
using MessagePtr = std::shared_ptr<Message>;

#pragma pack(push, 1)
struct MsgHead
{
    int32_t                     m_length;
    uint8_t                     m_sequence_id;
    uint8_t                     m_unknown_1;
    uint8_t                     m_unknown_2;
    uint8_t                     m_unknown_3;
    uint64_t                    m_uid;
    std::array<uint8_t, 8>      m_sid;
};
static_assert(std::is_pod<MsgHead>::type, "MsgHead must be POD!");

#pragma pack(pop)

enum { MSG_HEAD_SIZE = sizeof(MsgHead) };
