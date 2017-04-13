#ifndef _GAME_TOOLS_H_
#define _GAME_TOOLS_H_

#include <sys/time.h>

#include <cstdint>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

namespace tools {

int32_t diffMillisecond(const struct timeval& start, const struct timeval& end);

bool checkMemcpy(void* dest, const void* src, size_t src_len, size_t max);
bool checkMemcpy(void* dest, const char* src, size_t max);
bool checkMemcpy(void* dest, const std::string& src, size_t max);

//返回小于等于src，又能整除target的数
template <typename T1,
    typename T2,
    typename T3 = typename std::conditional<std::is_enum<T2>::value, int, T2>::type
    >
T1 alignIntegral(T1 src, T2 target)
{
    static_assert(std::is_integral<T1>::value, "T1 must integral!");
    static_assert(std::is_integral<T3>::value, "T2 must integral!");
    //GameAssert(target > 0);
    return src/target * T3(target);
}

} /// tools

#endif
