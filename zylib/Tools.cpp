#include "Tools.h"

#include <cstdlib>
#include <ctime>
#include <cstdarg>
#include <cstring>
#include <sstream>
#include <memory>
#include <sys/time.h>

namespace zylib {

bool isSameDay(time_t tTime1, time_t tTime2)
{
    struct tm *pTimes1;
    struct tm tmptime1;
    pTimes1 = localtime_r(&tTime1, &tmptime1);

    struct tm *pTimes2;
    struct tm tmptime2;
    pTimes2 = localtime_r(&tTime2, &tmptime2);
    return pTimes1->tm_year == pTimes2->tm_year &&
        pTimes1->tm_mon == pTimes2->tm_mon &&
        pTimes1->tm_mday == pTimes2->tm_mday;
}

bool isSameWeek(time_t tTime1,time_t tTime2)
{
	int wDay1 = getWday(tTime1);
	int wDay2 = getWday(tTime2);
	if (wDay2 == 0)
		wDay2 = 7;
	if (wDay1 == 0)
		wDay1 = 7;
	
	int year,month,day;
	timeToDate(tTime1,year,month,day);
	tTime1 = makeTime(year,month,day);
	timeToDate(tTime2,year,month,day);
	tTime2 = makeTime(year,month,day);

	return abs(static_cast<int>(tTime1) - static_cast<int>(tTime2)) == (abs(wDay1-wDay2) * 3600 * 24);
}

bool isSameMonth(time_t tTime1,time_t tTime2)
{
	struct tm *pTimes1;
	struct tm tmptime1;
	pTimes1 = localtime_r(&tTime1, &tmptime1);

	struct tm *pTimes2;
	struct tm tmptime2;
	pTimes2 = localtime_r(&tTime2, &tmptime2);
	return pTimes1->tm_year == pTimes2->tm_year &&
		pTimes1->tm_mon == pTimes2->tm_mon ;
}

DateTime timeToDateTime(time_t t)
{
    DateTime date_time;
    timeToDateTime(t, date_time.m_year, date_time.m_month, date_time.m_day,
        date_time.m_hour, date_time.m_minute, date_time.m_second, date_time.m_wday);
    return date_time;
}

void timeToDateTime(time_t t, int& year, int& month, int& day, int& hour, int& minute, int& second)
{
    struct tm tms;
    struct tm* ptm = localtime_r(&t, &tms);
    year = ptm->tm_year + 1900;
    month = ptm->tm_mon + 1;
    day = ptm->tm_mday;
    hour = ptm->tm_hour;
    minute = ptm->tm_min;
    second = ptm->tm_sec;
}

void timeToDateTime(time_t t, int& year, int& month, int& day, int& hour, int& minute, int& second, int& wday)
{
    struct tm tms;
    struct tm* ptm = localtime_r(&t, &tms);
    year = ptm->tm_year + 1900;
    month = ptm->tm_mon + 1;
    day = ptm->tm_mday;
    hour = ptm->tm_hour;
    minute = ptm->tm_min;
    second = ptm->tm_sec;
    wday = ptm->tm_wday + 1;
}

void timeToDate(time_t t, int& year, int& month, int& day)
{
    int h,min,sec;
    timeToDateTime(t, year, month, day, h, min, sec);
}

int getWday(time_t t)
{
	struct tm tms;
	struct tm* ptm = localtime_r(&t, &tms);

	return ptm->tm_wday;
}

int getMonth(time_t t)
{
    if (t <= 0)
        return 0;
    struct tm tms;
    struct tm* ptm = localtime_r(&t, &tms);
    return ptm->tm_mon + 1;
}

int getHour(time_t t)
{
    struct tm tms;
    struct tm* ptm = localtime_r(&t, &tms);
    return ptm->tm_hour;
}

int getMinute(time_t t)
{
    struct tm tms;
    struct tm* ptm = localtime_r(&t, &tms);
    return ptm->tm_min;
}

void timeToTime(time_t t, int& hour, int& minute, int& second)
{
    int y,m,d;
    timeToDateTime(t, y,m,d,hour,minute,second);
}

bool dateFromString(const char* str, int& year, int& month, int& day)
{
    int n = sscanf(str, "%d-%d-%d", &year, &month, &day);
    if (n != 3)
        return false;
    if (month < 1 || 12 < month)
        return false;
    if (day < 1 || 31 < day)
        return false;
    return true;
}

bool dateFromString(const char* str, int& year, int& month, int& day,
                    int& hour, int& minute, int& second)
{
    int n = sscanf(str, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    if (n != 6)
        return false;
    if (month < 1 || 12 < month)
        return false;
    if (day < 1 || 31 < day)
        return false;
    if (hour < 0 || 23 < hour)
        return false;
    if (minute < 0 || 59 < hour)
        return false;
    if (second < 0 || 59 < second)
        return false;
    return true;
}

time_t relativeTime(time_t t, int hour, int minute, int second)
{
    int year = 0;
    int mon = 0;
    int day = 0;
	int hh = 0;
	int mm = 0;
	int ss = 0;
    timeToDateTime(t, year, mon, day, hh, mm, ss);
    return makeTime(year, mon, day, hour, minute, second);
}

time_t makeTime(const DateTime& date_time)
{
    return makeTime(date_time.m_year,date_time.m_month, date_time.m_day,
        date_time.m_hour, date_time.m_minute, date_time.m_second);
}

time_t makeTime(int y, int m, int d, int hh, int mm, int ss)
{
    struct tm tms;
    tms.tm_year = y - 1900;
    tms.tm_mon = m - 1;
    tms.tm_mday = d;
    tms.tm_hour = hh;
    tms.tm_min = mm;
    tms.tm_sec = ss;
    return mktime(&tms);
}

std::string datetime(time_t t)
{
    int year, mon, day, hour, min, sec;
    timeToDate(t, year, mon, day);
    timeToTime(t, hour, min, sec);
    char buff[64];
    snprintf(buff, sizeof(buff), "%d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
    return std::string(buff);
}

time_t strToTime_t(const std::string & time_string)  
{  
	struct tm tm1;  
	time_t time1;  
	sscanf(time_string.c_str(), "%d-%d-%d %d:%d:%d" ,       
		&(tm1.tm_year),   
		&(tm1.tm_mon),   
		&(tm1.tm_mday),  
		&(tm1.tm_hour),  
		&(tm1.tm_min),  
		&(tm1.tm_sec) );  

	tm1.tm_year -= 1900;  
	tm1.tm_mon --;  
	tm1.tm_isdst=-1;  
	time1 = mktime(&tm1);  

	return time1;  
}  

int countDays(time_t tBegin, time_t tEnd )
{
	std::string strBegin = datetime(tBegin);
	std::string strEnd = datetime( tEnd );

	strEnd = strEnd.substr(0, 10 );
	strEnd += strBegin.substr(10);

	tEnd = strToTime_t(strEnd );

	int nDays = static_cast<int>((tEnd - tBegin ) / 86400);
	return nDays;
}

std::vector<std::string> stringSplit(const std::string& s, char c)
{
    std::vector<std::string> out;
    if (s.empty())
        return out;

    std::istringstream istm(s);
    std::string temp;
    while (std::getline(istm, temp, c)) {
        out.push_back(temp);
    }
    return out;
}

std::string stringPrintf(const char* format, ...)
{
    std::string ret(std::max(32UL, (long unsigned)strlen(format) * 2), '\0');
    ret.resize(0);

    va_list ap;
    va_start(ap, format);
    stringPrintfImpl(ret, format, ap);
    va_end(ap);
    return ret;
}

std::string& stringAppendf(std::string* output, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    stringPrintfImpl(*output, format, ap);
    va_end(ap);
    return *output;
}

void stringPrintf(std::string* output, const char* format, ...)
{
    output->clear();
    va_list ap;
    va_start(ap, format);
    stringPrintfImpl(*output, format, ap);
    va_end(ap);
}

void stringPrintfImpl(std::string& output, const char* format, va_list args)
{
    const std::string::size_type write_point = output.size();
    std::string::size_type remaining = output.capacity() - write_point;
    output.resize(output.capacity());

    va_list args_copy;
    va_copy(args_copy, args);
    int bytes_used = vsnprintf(&output[write_point], remaining, format,
        args_copy);
    va_end(args_copy);
    if (bytes_used < 0) {
        output = "";
        return;
    } else if ((std::string::size_type)bytes_used < remaining) {
        output.resize(write_point + bytes_used);
    } else {
        output.resize(write_point + bytes_used + 1);
        remaining = bytes_used + 1;
        va_list args_copy_ex;
        va_copy(args_copy_ex, args);
        bytes_used = vsnprintf(&output[write_point], remaining, format,
            args_copy_ex);
        va_end(args_copy_ex);
        if ((std::string::size_type)bytes_used + 1 != remaining) {
            output = "";
            return;
        }
        output.resize(write_point + bytes_used);
    }
}

size_t stringReplace(std::string* str, char src, char dest)
{
    size_t t = 0;
    std::transform(str->begin(), str->end(), str->begin(),
        [&t, src, dest](char& c)
        {
            if (c == src) {
                c = dest;
                ++t;
            }
            return c;
        } );
    return t;
}

}
