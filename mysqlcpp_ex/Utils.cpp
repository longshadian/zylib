#include "Utils.h"

#include <cstring>
#include <algorithm>

#include "DateTime.h"
#include "FieldMeta.h"

namespace mysqlcpp {

namespace util {

Tokenizer::Tokenizer(const std::string &src, const char sep)
    : m_str()
    , m_storage()
{
    m_str.resize(src.length() + 1);
    std::copy(src.begin(), src.end(), m_str.begin());

    char* pos_old = m_str.data();
    char* pos = m_str.data();
    for (;;) {
        if (*pos == sep) {
            if (pos_old != pos)
                m_storage.push_back(pos_old);

            pos_old = pos + 1;
            *pos = '\0';
        } else if (*pos == '\0') {
            if (pos_old != pos)
                m_storage.push_back(pos_old);
            break;
        }
        ++pos;
    }
}

/*
bool datetimeFromStringEx(DateTime* date_time
    , enum_field_types filed_type
    , const std::string& str)
{
    if (filed_type == MYSQL_TYPE_YEAR) {
        return stringToYear4(str, date_time);
    }
}

bool stringToDatetime(const std::string& str, DateTime* date_time);
bool stringToDate(const std::string& str, DateTime* date_time);
bool stringToTime(const std::string& str, DateTime* date_time);

bool stringToYear4(const std::string& str, DateTime* date_time)
{
    MYSQL_TIME tm{};
    std::memset(&tm, 0, sizeof(MYSQL_TIME));
    tm.year = 
}
*/
 
bool datetimeFromString(DateTime* date_time, const std::string& str)
{
    Tokenizer tk{ str, ' ' };
    if (tk.size() != 2)
        return false;
    return dateFromString(date_time, tk[0]) && timeFromString(date_time, tk[1]);
}

bool dateFromString(DateTime* date_time, const std::string& str)
{
    // 0000-00-00
    Tokenizer tk{ str, '-' };
    if (tk.size() != 3)
        return false;
    auto& mysql_time = date_time->getMYSQL_TIME();
    mysql_time.year = static_cast<decltype(mysql_time.year)>(std::strtol(tk[0], nullptr, 10));
    mysql_time.month = static_cast<decltype(mysql_time.month)>(std::strtol(tk[1], nullptr, 10));
    mysql_time.day = static_cast<decltype(mysql_time.day)>(std::strtol(tk[2], nullptr, 10));
    return true;
}

bool timeFromString(DateTime* date_time, const std::string& str)
{
    // 00:00:00
    Tokenizer tk{ str, ':' };
    if (tk.size() != 3)
        return false;
    auto& mysql_time = date_time->getMYSQL_TIME();
    mysql_time.hour = static_cast<decltype(mysql_time.hour)>(std::strtol(tk[0], nullptr, 10));
    mysql_time.minute = static_cast<decltype(mysql_time.minute)>(std::strtol(tk[1], nullptr, 10));
    mysql_time.second = static_cast<decltype(mysql_time.second)>(std::strtol(tk[2], nullptr, 10));
    return true;
}

void bindFiledsMeta(MYSQL_RES* mysql_res, std::vector<FieldMeta>* fields_data)
{
    unsigned int field_count = ::mysql_num_fields(mysql_res);
    MYSQL_FIELD* mysql_fields = ::mysql_fetch_fields(mysql_res);
    fields_data->reserve(field_count);
    for (unsigned int i = 0; i != field_count; ++i) {
        //保存列元数据
        fields_data->push_back(FieldMeta(&mysql_fields[i], i));
    }
}


} // util
} // mysqlcpp
