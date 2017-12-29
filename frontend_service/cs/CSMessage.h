#pragma once

#include <cstdint>
#include <type_traits>
#include <array>
#include <memory>

namespace cs 
{
#pragma pack(push, 1)
struct CSMsgHead
{
    int32_t                     m_length;
    uint8_t                     m_sequence_id;
    uint8_t                     m_unknown_1;
    uint8_t                     m_unknown_2;
    uint8_t                     m_unknown_3;
    std::array<uint8_t, 8>      m_sid;
    int32_t                     m_msg_id;
};
#pragma pack(pop)

static_assert(std::is_pod<CSMsgHead>::value, "std::is_pod<CSMsgHead>::value is false!");


enum {SID_LENTH = 8};
enum {HEAD_LENGTH = sizeof(CSMsgHead)};

} //cs
