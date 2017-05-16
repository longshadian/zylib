#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <sstream>

namespace zylib {

struct DateTime
{
    int m_year;
    int m_month;
    int m_day;
    int m_hour;
    int m_minute;
    int m_second;
    int m_wday;
};

template< typename T> struct TypeNull;

bool isSameDay(time_t tTime1, time_t tTime2);
bool isSameWeek(time_t tTime1,time_t tTime2);
bool isSameMonth(time_t tTime1,time_t tTime2);

DateTime timeToDateTime(time_t t);
void timeToDateTime(time_t t, int& year, int& month, int& day, int& hour, int& minute, int& second);
void timeToDateTime(time_t t, int& year, int& month, int& day, int& hour, int& minute, int& second, int& wday);
void timeToDate(time_t t, int& year, int& month, int& day);
void timeToTime(time_t t, int& hour, int& minute, int& second);

int getWday(time_t t);

int getMonth(time_t t);
int getHour(time_t t);
int getMinute(time_t t);

//fromat: "%d-%d-%d"
bool dateFromString(const char* fromat, int& year, int& month, int& day);

//format: "%d-%d-%d %d:%d:%d"
bool dateFromString(const char* fromat, int& year, int& month, int& day, int& hour, int& minute, int& second);

//时间t所在当天的固定时间点
time_t relativeTime(time_t t, int hour, int minute, int second);

time_t makeTime(const DateTime& date_time);
time_t makeTime(int y, int m, int d, int hh = 0, int mm = 0, int ss = 0);

//format: "%d-%02d-%02d %02d:%02d:%02d"
std::string datetime(time_t t);
time_t strToTime_t(const std::string & time_string);

int countDays(time_t tBegin, time_t tEnd );

std::vector<std::string> stringSplit(const std::string& s, char c);

std::string stringPrintf(const char* format, ...);
std::string& stringAppendf(std::string* output, const char* format, ...);
void stringPrintf(std::string* output, const char* format, ...);
void stringPrintfImpl(std::string& output, const char* format, va_list args);

//字符串替换,字符串str中的src字符替换成dest,返回替换个数
size_t stringReplace(std::string* str, char src, char dest);

template <class RandomAccessIterator>
void linear_random_shuffle(RandomAccessIterator first, RandomAccessIterator last)
{
    typename std::iterator_traits<RandomAccessIterator>::difference_type n = (last - first);
    if (n <= 0)
        return;
    while (--n) {
        std::swap(first[n], first[rand() % (n + 1)]);
    }
}

template <typename T>
inline
std::unique_ptr<T> make_unique()
{
    return std::unique_ptr<T>{ new T() };
}

//////////////////////////////////////////////////////////////////////////
}
