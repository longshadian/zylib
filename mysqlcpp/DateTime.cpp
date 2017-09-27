#include "DateTime.h"

#include <ctime>
#include <time.h>
#include <cstring>
#include <array>

#include "Utils.h"

namespace mysqlcpp {

DateTime::DateTime()
    : m_mysql_time()
{
    std::memset(&m_mysql_time, 0, sizeof(m_mysql_time));
}

DateTime::DateTime(time_t t)
    : DateTime()
{
    setMysqlTime(getLocaltime(t));
}

DateTime::DateTime(const timeval& t)
    :DateTime()
{
    setMysqlTime(getLocaltime(t.tv_sec));
    m_mysql_time.second_part = t.tv_usec;
}

DateTime::DateTime(const MYSQL_TIME& mysql_time)
    : m_mysql_time(mysql_time)
{
}

DateTime::DateTime(const DateTime& rhs)
    : m_mysql_time(rhs.m_mysql_time)
{
}

DateTime& DateTime::operator=(const DateTime& rhs)
{
    if (this != &rhs) {
        m_mysql_time = rhs.m_mysql_time;
    }
    return *this;
}

DateTime::DateTime(DateTime&& rhs)
    : m_mysql_time(std::move(rhs.m_mysql_time))
{
}

DateTime& DateTime::operator=(DateTime&& rhs)
{
    if (this != &rhs) {
        m_mysql_time = std::move(rhs.m_mysql_time);
    }
    return *this;
}


DateTime::~DateTime()
{

}

std::vector<uint8> DateTime::getBinary() const
{
    std::vector<uint8> buffer{};
    buffer.resize(sizeof(m_mysql_time));
    std::memcpy(buffer.data(), &m_mysql_time, buffer.size());
    return buffer;
}

const MYSQL_TIME& DateTime::getMYSQL_TIME() const
{
    return m_mysql_time;
}

MYSQL_TIME& DateTime::getMYSQL_TIME()
{
    return m_mysql_time;
}

std::string DateTime::getString() const
{
    std::array<char, 128> arr{};
    arr.fill(0);
    snprintf(arr.data(), arr.size(),"%04d-%02d-%02d %02d:%02d:%02d"
        , m_mysql_time.year, m_mysql_time.month, m_mysql_time.day
        , m_mysql_time.hour, m_mysql_time.minute, m_mysql_time.second
        );
    return std::string{arr.data()};
}

time_t DateTime::getTime() const
{
    if (isNull())
        return 0;
    struct tm tms{};
    std::memset(&tms, 0, sizeof(tms));
    tms.tm_year = m_mysql_time.year - 1900;
    tms.tm_mon = m_mysql_time.month - 1;
    tms.tm_mday = m_mysql_time.day;
    tms.tm_hour = m_mysql_time.hour;
    tms.tm_min = m_mysql_time.minute;
    tms.tm_sec = m_mysql_time.second;
    return std::mktime(&tms);
}

void DateTime::setMysqlTime(const std::array<unsigned long, 6>& arr)
{
    m_mysql_time.year   = static_cast<decltype(m_mysql_time.year)>(arr[0]);
    m_mysql_time.month  = static_cast<decltype(m_mysql_time.month)>(arr[1]);
    m_mysql_time.day    = static_cast<decltype(m_mysql_time.day)>(arr[2]);
    m_mysql_time.hour   = static_cast<decltype(m_mysql_time.hour)>(arr[3]);
    m_mysql_time.minute = static_cast<decltype(m_mysql_time.minute)>(arr[4]);
    m_mysql_time.second = static_cast<decltype(m_mysql_time.second)>(arr[5]);
    m_mysql_time.second_part = 0;
    m_mysql_time.neg = 0;
    m_mysql_time.time_type = MYSQL_TIMESTAMP_DATETIME;
}

std::array<unsigned long, 6> DateTime::getLocaltime(time_t t)
{
    std::array<unsigned long, 6> val{0};
    struct tm tms;
    struct tm* ptm = localtime_r(&t, &tms);
    //localtime_s(&tms, &t);
    //struct tm* ptm = &tms;
    val[0] = ptm->tm_year + 1900;
    val[1] = ptm->tm_mon + 1;
    val[2] = ptm->tm_mday;
    val[3] = ptm->tm_hour;
    val[4] = ptm->tm_min;
    val[5] = ptm->tm_sec;
    return val;
}

bool DateTime::isNull() const
{
    return m_mysql_time.year == 0 
        && m_mysql_time.month == 0
        && m_mysql_time.day == 0
        && m_mysql_time.hour == 0
        && m_mysql_time.minute == 0
        && m_mysql_time.second == 0
        && m_mysql_time.second_part == 0;
}

}

