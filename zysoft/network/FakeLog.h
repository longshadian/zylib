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

const char* SeverityString(int severity)
{
    const char* severity_str;
    switch (severity) 
    {
    case ESeverity::EDebug:
        severity_str = "DEBUG";
        break;
    case ESeverity::EInfo:
        severity_str = "INFO";
        break;
    case ESeverity::EWarning:
        severity_str = "WARNING";
        break;
    case ESeverity::EError:
        severity_str = "ERROR";
        break;
    default:
        severity_str = "???";
        break;
    }
    return severity_str;
}



typedef void(*LogCallback)(int severity, const char* msg);
void SetLogCallback(LogCallback cb)
{
    static LogCallback g_log_callback = cb;
}

void PrintfLog(int severity, const char* fmt, ...)
#ifdef __GNUC__
__attribute__((format(printf, 2, 3)))
#endif
{
    va_list ap;
    va_start(ap, fmt);
    PrintfLogv(severity, nullptr, fmt, ap);
    va_end(ap);
}


namespace detail
{

static int Snprintf(char* buf, std::size_t buflen, const char* format, ...)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 4)))
#endif
    ;

static int Vsnprintf(char* buf, std::size_t buflen, const char* format, va_list ap)
#ifdef __GNUC__
        __attribute__((format(printf, 3, 0)))
#endif
    ;

void FlushLog(int severity, const char *msg)
{
    if (g_log_callback) {
        g_log_callback(severity, msg);
    }
}

void PrintfLogv(int severity, const char* errstr, const char* fmt, va_list ap)
{
    char buf[1024];
    std::size_t len;

    if (fmt)
        detail::Vsnprintf(buf, sizeof(buf), fmt, ap);
    else
        buf[0] = '\0';

    if (errstr) {
        len = std::strlen(buf);
        if (len < sizeof(buf) - 3) {
            detail::Snprintf(buf + len, sizeof(buf) - len, ": %s", errstr);
        }
    }
    FlushLog(severity, buf);
}

int Snprintf(char *buf, size_t buflen, const char *format, ...)
{
    int r;
    va_list ap;
    va_start(ap, format);
    r = Vsnprintf(buf, buflen, format, ap);
    va_end(ap);
    return r;
}

int Vsnprintf(char* buf, size_t buflen, const char *format, va_list ap)
{
    int r;
    if (!buflen)
        return 0;
#if defined(_MSC_VER) || defined(_WIN32)
    r = _vsnprintf_s(buf, buflen, buflen, format, ap);
    if (r < 0)
        r = _vscprintf(format, ap);
    r = vsnprintf(buf, buflen, format, ap);
#endif
    buf[buflen - 1] = '\0';
    return r;
}

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
