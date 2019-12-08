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


void Gettimeofday(struct timeval* tp)
{
#if defined(_WIN32)
    uint64_t intervals;
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);

    /*
     * A file time is a 64-bit value that represents the number
     * of 100-nanosecond intervals that have elapsed since
     * January 1, 1601 12:00 A.M. UTC.
     *
     * Between January 1, 1970 (Epoch) and January 1, 1601 there were
     * 134744 days,
     * 11644473600 seconds or
     * 11644473600,000,000,0 100-nanosecond intervals.
     *
     * See also MSKB Q167296.
     */

    intervals = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    tp->tv_sec = (long)(intervals / 10000000);
    tp->tv_usec = (long)((intervals % 10000000) / 10);
#else
    ::gettimeofday(tp, nullptr);
#endif // defined(_WIN32)
}



struct tm* Localtime(const time_t* t, struct tm* output)
{
#if defined(_WIN32)
    localtime_s(output, t);
#else
    localtime_r(t, output);
#endif
    return output;
}

std::string Localtime_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    Gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    Localtime(&cur_t, &cur_tm);

    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%06d",
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
}
