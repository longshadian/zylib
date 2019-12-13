#pragma once

#include <cstdint>
#include <cstddef>
#include <ctime>
#include <string>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace zysoft
{

inline void gmtime(const std::time_t* t, struct tm* output)
{
#if defined(_WIN32)
    ::gmtime_s(output, t);
#else
    ::gmtime_r(t, output);
#endif
}

inline void gettimeofday(struct timeval* tp)
{
#if defined(_WIN32)
    std::uint64_t intervals;
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

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

    intervals = ((std::uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    tp->tv_sec = (long)(intervals / 10000000);
    tp->tv_usec = (long)((intervals % 10000000) / 10);
#else
    ::gettimeofday(tp, nullptr);
#endif // defined(_WIN32)
}

inline struct tm* Localtime(const time_t* t, struct tm* output)
{
#if defined(_WIN32)
    ::localtime_s(output, t);
#else
    ::localtime_r(t, output);
#endif
    return output;
}

inline std::string Localtime_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    Localtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d"
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

inline std::string Localtime_YYYYMMDD_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    Localtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d"
        , cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

inline std::string Localtime_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    gettimeofday(&cur_tv);

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

inline std::string Localtime_YYYYMMDD_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    Localtime(&cur_t, &cur_tm);
    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d.%06d",
        cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday,
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
}

inline std::string UTC_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    gmtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d"
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

inline std::string UTC_YYYYMMDD_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    gmtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d"
        , cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

inline std::string UTC_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    gmtime(&cur_t, &cur_tm);

    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%06d",
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
}

inline std::string UTC_YYYYMMDD_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    gmtime(&cur_t, &cur_tm);
    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d.%06d",
        cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday,
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
}


} // namespace zysoft
