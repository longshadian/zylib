#include "GameTools.h"

#include <vector>
#include <cstdint>
#include <sstream>
#include <algorithm>

namespace tools {

int32_t diffMillisecond(const struct timeval& start, const struct timeval& end)
{
    int32_t t = static_cast<int32_t>((end.tv_sec - start.tv_sec) * 1000);
    t += static_cast<int32_t>((end.tv_usec - start.tv_usec)/1000);
    return t;
}

bool checkMemcpy(void* dest, const void* src, size_t src_len, size_t max)
{
    if (src_len > max)
        return false;
    std::memset(dest, 0, max);
    std::memcpy(dest, src, src_len);
    return true;
}

bool checkMemcpy(void* dest, const char* src, size_t max)
{
    return checkMemcpy(dest, src, std::strlen(src), max);
}

bool checkMemcpy(void* dest, const std::string& src, size_t max)
{
    return checkMemcpy(dest, src.data(), src.length(), max);
}

}
