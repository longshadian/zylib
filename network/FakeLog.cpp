#include "FakeLog.h"

#include <iostream>

#define FAKE_LOG_APPLOG

#ifdef FAKE_LOG_APPLOG
#include "AppLog.h"
#endif

FakeLog::FakeLog(int32_t lv, int line, const char* file, const char* function)
    : m_line(line)
    , m_file(file)
    , m_function(function)
    , m_level(lv)
    , m_stream()
{
}

FakeLog::~FakeLog()
{
    auto s = m_stream.m_ostm.str();
    if (s.empty())
        return;
#ifdef FAKE_LOG_APPLOG
    basicLog(LOG_DEBUG, "%d:%s:%s %s", m_line, m_file, m_function, s.c_str());
#endif
}
