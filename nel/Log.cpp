#include "Log.h"

#include <iostream>

namespace nlnet {
    
LogStream::LogStream()
    : m_mtx()
    , m_ostm()
{
}

LogStream::~LogStream()
{
}


LogStream& LogStream::operator<<(signed char val) 
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(unsigned char val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(char val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(short val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(unsigned short val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(int val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(unsigned int val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(long val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(unsigned long val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(long long val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(float val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(double val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(long double val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(const std::string& val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

LogStream& LogStream::operator<<(const char* val)
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm << val;
    return *this;
}

void LogStream::flush()
{
    std::lock_guard<std::mutex> lk{m_mtx};
    m_ostm.str("");
}


const char* LOG_SEVERITY_NAMES[NUM_SEVERITY] =
{
	"[DEBUG  ]",
	"[INFO   ]",
	"[WARNING]",
	"[ERROR  ]",
};

std::unique_ptr<LogStream> g_ostm = nullptr;
LOG_LEVEL g_level = DEBUG;

void initLog(std::unique_ptr<LogStream> ostm, LOG_LEVEL lv)
{
    if (!g_ostm)
        g_ostm = std::move(ostm);
    g_level = lv;
}

FakeLog::FakeLog(int lv, const char* file, int line, const char* function)
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
    /*
    if (!g_ostm || m_level < g_level)
        return;
        */
    auto content = m_stream.m_ostm.str();
    std::cout << LOG_SEVERITY_NAMES[m_level] << " [" << m_file << ":" << m_line << ":" << m_function << "] " 
        << content << "\n";
    /*
	(*g_ostm) << LOG_SEVERITY_NAMES[m_level] << " [" << m_file << ":" << m_line << "] " << content;
    g_ostm->flush();
    */
}

}
