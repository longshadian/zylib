#include "network/Buffer.h"

bool FlatBuffer::Empty() const
{
    return m_buffer.empty();
}

std::size_t FlatBuffer::Length() const
{
    return m_buffer.size();
}

const std::uint8_t* FlatBuffer::Ptr() const
{
    return m_buffer.data();
}

std::uint8_t* FlatBuffer::Ptr()
{
    return m_buffer.data();
}

void FlatBuffer::Append(const void* data, std::size_t length)
{
    if (!data || length == 0)
        return;
    const std::uint8_t* pos = reinterpret_cast<const std::uint8_t*>(data);
    m_buffer.insert(m_buffer.end(), pos, pos + length);
}

bool FlatBuffer::Consume(std::size_t length)
{
    if (m_buffer.size() < length)
        return false;
    std::size_t remain = m_buffer.size() - length;
    auto* pos = m_buffer.data();
    std::memmove(pos, pos + length, remain);
    m_buffer.resize(remain);
    return true;
}

void FlatBuffer::Resize(std::size_t length)
{
    m_buffer.resize(Length());
}
