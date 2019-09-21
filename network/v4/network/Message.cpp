#include "network/Message.h"

#include <cstring>
#include <array>

namespace network
{

Message::Message()
    : m_buffer()
{
}

Message::~Message()
{
}

Message::Message(const Message& rhs)
    : m_buffer(rhs.m_buffer)
{
}

Message& Message::operator=(const Message& rhs)
{
    if (this != &rhs) {
        this->m_buffer = rhs.m_buffer;
    }
    return *this;
}

Message::Message(Message&& rhs)
    : m_buffer(std::move(rhs.m_buffer))
{
}

Message& Message::operator=(Message&& rhs)
{
    if (this != &rhs) {
        std::swap(this->m_buffer, rhs.m_buffer);
    }
    return *this;
}

bool Message::Empty() const
{
    return m_buffer.Empty();
}

std::size_t Message::Length() const
{
    return m_buffer.Length();
}

bool Message::GetHead(MessageHead* head) const
{
    if (Length() < HeadLength())
        return false;
    const auto* pos = HeadPtr();
    std::memcpy(&head, pos, HeadLength());
    return true;
}

const std::uint8_t* Message::HeadPtr() const
{
    return m_buffer.Ptr();
}

const std::uint8_t* Message::BodyPtr() const
{
    return m_buffer.Ptr() + HEAD_LENGTH;
}

std::size_t Message::HeadLength() const
{
    return HEAD_LENGTH;
}

std::size_t Message::BodyLength() const
{
    if (m_buffer.Length() < HEAD_LENGTH)
        return 0;
    return m_buffer.Length() - HEAD_LENGTH;
}

} // namespace network

