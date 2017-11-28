#pragma once

#include <sstream>
#include <memory>
#include <mutex>

namespace knet {

class LogStream
{
public:
    LogStream() = default;
    virtual ~LogStream() = default;

    LogStream(const LogStream&) = delete;
    LogStream& operator=(const LogStream&) = delete;
    LogStream(LogStream&&) = delete;
    LogStream& operator=(LogStream&&) = delete;

    virtual void flush(std::string str) = 0;
};

enum LOG_LEVEL
{
	DEBUG	= 0,
	INFO	= 1,
	WARNING = 2,
	ERROR	= 3,
	NUM_SEVERITY = 4,
};

extern const char* LOG_SEVERITY_NAMES[NUM_SEVERITY];

void initLog(std::unique_ptr<LogStream> ostm, LOG_LEVEL lv = DEBUG);

struct FakeLog
{
    FakeLog(LOG_LEVEL lv, const char* file, int line, const char* fun);
    ~FakeLog();
    FakeLog(const FakeLog&) = delete;
    FakeLog& operator=(const FakeLog&) = delete;
    FakeLog(FakeLog&&) = delete;
    FakeLog& operator=(FakeLog&&) = delete;

    std::ostringstream& stream() { return m_stream; }
private:
    int             m_line;
    const char*     m_file;
    const char*     m_function;
    LOG_LEVEL       m_level;
    std::ostringstream m_stream;
};

} // knet 

#define FAKE_LOG(type) knet::FakeLog(knet::type, __FILE__, __LINE__, __FUNCTION__).stream()
