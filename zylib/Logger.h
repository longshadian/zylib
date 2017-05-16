#pragma once

#include <string>

#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/async_frontend.hpp>

//#define BOOST_LOG_DYN_LINK 1

#define LOG(log_type) \
    BOOST_LOG_SEV(zylib::logger::Logger::getInstance().m_lg, zylib::logger::log_type) \
        << __FILE__ << ':' << __LINE__ << ':' << __FUNCTION__ << "] "

#define LOG_FMT(log_type, format, ...) \
    logFormat(zylib::logger::log_type, "%s:%d:%s] " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

namespace zylib {
namespace logger {

enum LOG_TYPE
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
};

template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, LOG_TYPE lvl)
{
    static const char* const str[] = {
        "DEBUG  ",
        "INFO   ",
        "WARNING",
        "ERROR  ",
    };
    if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast<int>(lvl);
    return strm;
}

struct LogOptional
{
    bool                    m_auto_flush{true};
    std::ios_base::openmode m_open_mode{std::ios::app};
    std::string             m_file_name_pattern{"./server_%Y-%m-%d_%H.%3N.log"};
    uint32_t                m_rotation_size{1024 * 1024 * 100};
    boost::log::sinks::file::rotation_at_time_point m_rotation_at_time_point{0, 0, 0};
};

void init(std::string path);
void init(const LogOptional& opt);
void stop();

#ifdef WIN32
 void logFormat(LOG_TYPE log_type, const char* format, ...);
#else
 void logFormat(LOG_TYPE log_type, const char* format, ...) __attribute__((format(printf, 2, 3)));
#endif // WIN32

typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_file_backend> FileSink;

class Logger
{
    Logger();
public:
    ~Logger();
    static Logger& getInstance();
    void init(const LogOptional& opt);
    void stop();
private:
    boost::shared_ptr<FileSink>  m_sink;
public:
    boost::log::sources::severity_logger<LOG_TYPE> m_lg;
};


}
}
