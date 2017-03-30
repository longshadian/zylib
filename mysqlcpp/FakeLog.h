#ifndef _MYSQLCPP_FAKELOG_H
#define _MYSQLCPP_FAKELOG_H

#include <sstream>

namespace mysqlcpp {

enum LOG_LEVEL
{
	LOG_DEBUG	= 0,
	LOG_INFO	= 1,
	LOG_WARNING = 2,
	LOG_ERROR	= 3,
	NUM_SEVERITY = 4,
};

extern const char* LOG_SEVERITY_NAMES[NUM_SEVERITY];



void initLog(std::ostream* ostm);

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

#define FAKE_LOG_DEBUG()	FakeLog(LOG_DEBUG,		__FILE__, __LINE__).stream()
#define FAKE_LOG_INFO()		FakeLog(LOG_INFO,		__FILE__, __LINE__).stream()
#define FAKE_LOG_WARNING()	FakeLog(LOG_WARNING,	__FILE__, __LINE__).stream()
#define FAKE_LOG_ERROR()	FakeLog(LOG_ERROR,		__FILE__, __LINE__).stream()

}


#endif
