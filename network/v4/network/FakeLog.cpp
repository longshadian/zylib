#include "network/FakeLog.h"

#include <cstring>

namespace network
{
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

} // namespace detail
} // namespace network



namespace network
{
static void FlushLog(int severity, const char *msg);
static void PrintfLogv(int severity, const char* errstr, const char* fmt, va_list ap);

static LogCallback g_log_callback = nullptr;

void SetLogCallback(LogCallback cb)
{
    g_log_callback = cb;
}

void FlushLog(int severity, const char *msg)
{
    if (g_log_callback) {
        g_log_callback(severity, msg);
    }
}

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

void PrintfLog(int severity, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    PrintfLogv(severity, nullptr, fmt, ap);
    va_end(ap);
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

} // namespace network


namespace network
{
namespace detail 
{

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

