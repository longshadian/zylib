#pragma once

#include <cstdint>
#include <vector>

namespace network {

class FlatBuffer {
public:
    FlatBuffer() = default;
    ~FlatBuffer() = default;
    FlatBuffer(const FlatBuffer& rhs) = default;
    FlatBuffer& operator=(const FlatBuffer& rhs) = default;
    FlatBuffer(FlatBuffer&& rhs) = default;
    FlatBuffer& operator=(FlatBuffer&& rhs) = default;

    bool Empty() const
    {
        return m_buffer.empty();
    }

    std::size_t Length() const
    {
        return m_buffer.size();
    }

    const std::uint8_t* Ptr() const
    {
        return m_buffer.data();
    }

    std::uint8_t* Ptr()
    {
        return m_buffer.data();
    }


    void Append(const void* data, std::size_t length)
    {
        if (!data || length == 0)
            return;
        const std::uint8_t* pos = reinterpret_cast<const std::uint8_t*>(data);
        m_buffer.insert(m_buffer.end(), pos, pos + length);
    }


    bool Consume(std::size_t length)
    {
        if (m_buffer.size() < length)
            return false;
        std::size_t remain = m_buffer.size() - length;
        auto* pos = m_buffer.data();
        std::memmove(pos, pos + length, remain);
        m_buffer.resize(remain);
        return true;
    }

    void Resize(std::size_t length)
    {
        m_buffer.resize(length);
    }

    void Reverse(std::size_t length)
    {
        m_buffer.reserve(length);
    }

    void Clear()
    {
        m_buffer.clear();
    }

private:
    std::vector<std::uint8_t> m_buffer;
};

} // namespace network
