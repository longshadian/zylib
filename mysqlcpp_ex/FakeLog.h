#ifndef _MYSQLCPP_FAKELOG_H
#define _MYSQLCPP_FAKELOG_H

#include <sstream>

namespace mysqlcpp {

#undef ERROR

enum LOG_LEVEL
{
	DEBUG	= 0,
	INFO	= 1,
	WARNING = 2,
	ERROR	= 3,
	NUM_SEVERITY = 4,
};

extern const char* LOG_SEVERITY_NAMES[NUM_SEVERITY];


void initLog(std::ostream* ostm, LOG_LEVEL lv = DEBUG);

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
    FakeLog(int lv, int line, const char* file, const char* function);
    FakeLog(int lv, const char* file, int line);
    ~FakeLog();

    FakeLogStream& stream() { return m_stream; }
private:
    int             m_line;
    const char*     m_file;
    const char*     m_function;
    int				m_level;
    FakeLogStream   m_stream;
};

#define FAKE_LOG(type)	FakeLog(type,		__FILE__, __LINE__).stream()

}


#endif
