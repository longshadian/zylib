#include "FakeLog.h"

#include <iostream>

namespace fake_log {
    
const char* LOG_SEVERITY_NAMES[NUM_SEVERITY] =
{
	"[FAKE_LOG_DEBUG  ]",
	"[FAKE_LOG_INFO   ]",
	"[FAKE_LOG_WARNING]",
	"[FAKE_LOG_ERROR  ]",
};

std::unique_ptr<LogStream> g_ostm = nullptr;
LOG_LEVEL g_level = DEBUG;

void initLog(std::unique_ptr<LogStream> ostm, LOG_LEVEL lv)
{
    if (!g_ostm)
        g_ostm = std::move(ostm);
    g_level = lv;
}

FakeLog::FakeLog(LOG_LEVEL lv, const char* file, int line, const char* fun)
    : m_line(line)
    , m_file(file)
    , m_function(fun)
    , m_level(lv)
    , m_stream()
{
    if (!g_ostm || m_level < g_level) {
        return;
    }
    stream() << LOG_SEVERITY_NAMES[m_level] << " [" << m_file << ":" << m_line << "] ";
}

FakeLog::~FakeLog()
{
    if (!g_ostm || m_level < g_level)
        return;
    g_ostm->flush(m_stream.str());
}

} // fake_log
