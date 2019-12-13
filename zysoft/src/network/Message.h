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
    Message()
        : m_buffer()
    {
    }

    ~Message()
    {
    }

    Message(const Message& rhs)
        : m_buffer(rhs.m_buffer)
    {
    }

    Message& operator=(const Message& rhs)
    {
        if (this != &rhs) {
            this->m_buffer = rhs.m_buffer;
        }
        return *this;
    }

    Message(Message&& rhs)
        : m_buffer(std::move(rhs.m_buffer))
    {
    }

    Message& operator=(Message&& rhs)
    {
        if (this != &rhs) {
            std::swap(this->m_buffer, rhs.m_buffer);
        }
        return *this;
    }

    bool Empty() const
    {
        return m_buffer.Empty();
    }

    std::size_t Length() const
    {
        return m_buffer.Length();
    }

    bool GetHead(MessageHead* head) const
    {
        if (Length() < HeadLength())
            return false;
        const auto* pos = HeadPtr();
        std::memcpy(&head, pos, HeadLength());
        return true;
    }

    const std::uint8_t* HeadPtr() const
    {
        return m_buffer.Ptr();
    }

    const std::uint8_t* BodyPtr() const
    {
        return m_buffer.Ptr() + HEAD_LENGTH;
    }

    std::size_t HeadLength() const
    {
        return HEAD_LENGTH;
    }

    std::size_t BodyLength() const
    {
        if (m_buffer.Length() < HEAD_LENGTH)
            return 0;
        return m_buffer.Length() - HEAD_LENGTH;
    }
    
    FlatBuffer m_buffer;
};

} // namespace network
