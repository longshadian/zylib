#include "TestDefine.h"

#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#endif

bool IsEqual(const std::vector<std::uint8_t>& s1, const std::vector<std::uint8_t>& s2)
{
    return IsEqual(s1.data(), s1.size(), s2.data(), s2.size());
}

bool IsEqual(const void* s1, std::size_t s1_len, const void* s2, std::size_t s2_len)
{
    if (s1_len != s2_len)
        return false;
    return std::equal((const std::uint8_t*)s1, (const std::uint8_t*)s1 + s1_len, (const std::uint8_t*)s2);
}
