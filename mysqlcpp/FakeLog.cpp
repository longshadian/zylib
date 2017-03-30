#include "FakeLog.h"

#include <iostream>

namespace mysqlcpp {

const char* LOG_SEVERITY_NAMES[NUM_SEVERITY] =
{
	"DEBU   ",
	"INFO	",
	"WARNING",
	"ERROR  ",
};


std::ostream* g_ostm = nullptr;

void initLog(std::ostream* ostm)
{
    if (!g_ostm)
        g_ostm = ostm;
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
    auto content = m_stream.m_ostm.str();
    if (content.empty() || !g_ostm)
        return;
	if (content[content.size() - 1] == '\n')
		content.pop_back();
	(*g_ostm) << '[' << LOG_SEVERITY_NAMES[m_level] << "] [" << m_file << ":" << m_line << "] " << content << "\n";
}

}
