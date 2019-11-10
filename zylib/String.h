#pragma once

#include <cstdint>
#include <cstddef>
#include <string>

#include "zylib/PlatformDefine.h"

namespace zylib {

template <typename T>
T Tolower(T c)
{
    return static_cast<T>((c >= 'A' && c <= 'Z') ? (c | 0x20) : c);
}


template <typename T>
T Toupper(T c)
{
    return static_cast<T>((c >= 'a' && c <= 'z') ? (c & ~0x20) : c);
}

std::uint8_t* HexDump(std::uint8_t* dst, std::uint8_t* src, std::size_t len);

std::size_t ZYLIB_INLINE Base64EncodedLength(std::size_t len)
{
    return ((len + 2) / 3) * 4;
}

std::size_t ZYLIB_INLINE Base64DecodedLength(std::size_t len)
{
    return ((len + 3) / 4) * 3;
}

#if 0
std::size_t Base64Encode(const std::uint8_t* src, std::size_t src_len, void* dst, std::size_t* dst_len = nullptr);
#endif
void Base64Encode(const void* src, std::size_t src_len, void* dst, std::size_t* dst_len = nullptr);
void Base64Encode(const void* src, std::size_t src_len, std::string* dst);
std::int32_t Base64Decode(const void* src, std::size_t src_len, void* dst, std::size_t* dst_len = nullptr);


} // namesapce zylib
