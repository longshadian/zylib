#include "FakeLog.h"

#include <iostream>

namespace mysqlcpp {

const char* LOG_SEVERITY_NAMES[NUM_SEVERITY] =
{
	"MYSQLCPP_DEBU   ",
	"MYSQLCPP_INFO	 ",
	"MYSQLCPP_WARNING",
	"MYSQLCPP_ERROR  ",
};


std::ostream* g_ostm = nullptr;
LOG_LEVEL g_level = NUM_SEVERITY;

void initLog(std::ostream* ostm, LOG_LEVEL lv)
{
    if (!g_ostm)
        g_ostm = ostm;
    g_level = lv;
}

FakeLog::FakeLog(int lv, int line, const char* file, const char* function)
    : m_line(line)
    , m_file(file)
    , m_function(function)
    , m_level(lv)
    , m_stream()
{
}

FakeLog::FakeLog(int lv, const char* file, int line)
	: m_line(line)
	, m_file(file)
	, m_function(nullptr)
	, m_level(lv)
	, m_stream()
{
}

FakeLog::~FakeLog()
{
    if (!g_ostm || m_level < g_level)
        return;
    auto content = m_stream.m_ostm.str();
    if (!content.empty()) {
        if (content[content.size() - 1] == '\n')
            content.pop_back();
    }
	(*g_ostm) << '[' << LOG_SEVERITY_NAMES[m_level] << "] [" << m_file << ":" << m_line << "] " << content << "\n";
}

}
