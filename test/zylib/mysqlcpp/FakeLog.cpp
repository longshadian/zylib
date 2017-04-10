#include "FakeLog.h"

#include <iostream>

namespace mysqlcpp {

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

    if (m_level == FAKE_LOG_DEBUG) {
        std::cout << "[LOG_INFO ]\t" << s << "\n";
    } else if (m_level == FAKE_LOG_ERROR) {
        std::cout << "[LOG_ERROR]\t" << s << "\n";
    }
}

}
