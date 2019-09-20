#pragma once

#include <cstdint>
#include <vector>

namespace network
{

class FlatBuffer
{
public:
    FlatBuffer() = default;
    ~FlatBuffer() = default;
    FlatBuffer(const FlatBuffer& rhs) = default;
    FlatBuffer& operator=(const FlatBuffer& rhs) = default;
    FlatBuffer(FlatBuffer&& rhs) = default;
    FlatBuffer& operator=(FlatBuffer&& rhs) = default;

    bool Empty() const;
    std::size_t Length() const;
    const std::uint8_t* Ptr() const;
    std::uint8_t* Ptr();
    void Append(const void* data, std::size_t length);
    bool Consume(std::size_t length);
    void Resize(std::size_t length);
    void Reverse(std::size_t length);
    void Clear();

private:
    std::vector<std::uint8_t> m_buffer;
};

} // namespace network
