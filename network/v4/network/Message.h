#pragma once

#include <cstdint>
#include <chrono>
#include <memory>
#include <vector>
#include <string>

#include "network/Buffer.h"

namespace network
{

#pragma pack(push, 1)
struct MessageHead
{
    std::int32_t    m_body_length;
    std::int32_t    m_unknown_0;
    std::int32_t    m_unknown_1;
    std::int32_t    m_unknown_2;
};
#pragma pack(pop)
static_assert(std::is_pod<MessageHead>::value, "std::is_pod<MessageHead>::value is false!");

enum { HEAD_LENGTH = sizeof(MessageHead) };

class Message
{
public:
    Message();
    ~Message();
    Message(const Message& rhs);
    Message& operator=(const Message& rhs);
    Message(Message&& rhs);
    Message& operator=(Message&& rhs);

    bool Empty() const;
    std::size_t Length() const;
    void GetHead(MessageHead* head) const;
    const std::uint8_t* HeadPtr() const;
    const std::uint8_t* BodyPtr() const;
    std::size_t HeadLength() const;
    std::size_t BodyLength() const;
    
    FlatBuffer m_buffer;
};

} // namespace network
