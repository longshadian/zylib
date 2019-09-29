#pragma once

#include <cstdio>
#include <cstdarg>

namespace network
{

enum ESeverity
{
    EDebug = 0,
    EInfo = 1,
    EWarning = 2,
    EError = 3,
};

const char* SeverityString(int sev);

typedef void(*LogCallback)(int severity, const char* msg);
void SetLogCallback(LogCallback cb);


void PrintfLog(int severity, const char* fmt, ...)
#ifdef __GNUC__
__attribute__((format(printf, 3, 4)))
#endif
;

} // namespace network


namespace network
{
namespace detail
{

int Snprintf(char* buf, std::size_t buflen, const char* format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 4)))
#endif
    ;

int Vsnprintf(char* buf, std::size_t buflen, const char* format, va_list ap)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 0)))
#endif
    ;


} // namespace detail
} // namespace network


#if defined(NETWORK_DEV)
// #define NETWORK_DPrintf(fmt, ...) do { fprintf(stdout, "[%4d] [DEBUG  ] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
// #define NETWORK_IPrintf(fmt, ...) do { fprintf(stdout, "[%4d] [DEBUG  ] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
// #define NETWORK_WPrintf(fmt, ...) do { fprintf(stdout, "[%4d] [DEBUG  ] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
// #define NETWORK_EPrintf(fmt, ...) do { fprintf(stdout, "[%4d] [WARNING] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
#else
#define NETWORK_LOGFX(s, fmt, ...) network::PrintfLog(s, fmt, ##__VA_ARGS__)

#define NETWORK_DPrintf(fmt, ...) NETWORK_LOGFX(network::EDebug, fmt, ##__VA_ARGS__)
#define NETWORK_IPrintf(fmt, ...) NETWORK_LOGFX(network::EInfo, fmt, ##__VA_ARGS__)
#define NETWORK_WPrintf(fmt, ...) NETWORK_LOGFX(network::EWarning, fmt, ##__VA_ARGS__)
#define NETWORK_EPrintf(fmt, ...) NETWORK_LOGFX(network::EError, fmt, ##__VA_ARGS__)
#endif
