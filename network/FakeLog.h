#pragma once

#include <sstream>

struct FakeLogStream
{
    std::ostringstream m_ostm;
};

template <typename T>
inline FakeLogStream& operator<<(FakeLogStream& os, const T& t)
{
    (void)t;
    os.m_ostm << t;
    return os;
};

struct FakeLog
{
    enum FAKE_LOG_LEVEL
    {
        FAKE_LOG_ERROR = 0,
        FAKE_LOG_DEBUG = 1,
    };

    FakeLog(int32_t lv, int line, const char* file, const char* function);
    ~FakeLog();

    FakeLogStream& stream() { return m_stream; }
private:
    int             m_line;
    const char*     m_file;
    const char*     m_function;
    int32_t         m_level;
    FakeLogStream   m_stream;
};

#define FAKE_LOG_DEBUG() FakeLog(FakeLog::FAKE_LOG_DEBUG, __LINE__, __FILE__, __FUNCTION__).stream()
#define FAKE_LOG_ERROR() FakeLog(FakeLog::FAKE_LOG_ERROR, __LINE__, __FILE__, __FUNCTION__).stream()

