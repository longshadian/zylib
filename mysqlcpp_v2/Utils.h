#ifndef _MYSQLCPP_UTILS_H
#define _MYSQLCPP_UTILS_H

#include <mysql.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include "Types.h"

namespace mysqlcpp {

class DateTime;

namespace util {

class Tokenizer
{
public:
    typedef std::vector<char const*> StorageType;
    typedef StorageType::size_type size_type;
    typedef StorageType::const_iterator const_iterator;
    typedef StorageType::reference reference;
    typedef StorageType::const_reference const_reference;
public:
    Tokenizer(const std::string &src, char sep);
    ~Tokenizer() = default;

    const_iterator begin() const { return m_storage.begin(); }
    const_iterator end() const { return m_storage.end(); }

    bool empty() const { return m_storage.empty(); }
    size_type size() const { return m_storage.size(); }
    reference operator [] (size_type i) { return m_storage[i]; }
    const_reference operator [] (size_type i) const { return m_storage[i]; }
private:
    std::vector<char> m_str;
    StorageType m_storage;
};

bool stringTo_Date(const std::string& str, unsigned int* year, unsigned int* month, unsigned int* day);
bool stringTo_DateTime_Timestamp(const std::string& str
    , unsigned int* year, unsigned int* month, unsigned int* day
    , unsigned int* hour, unsigned int* minute, unsigned int* second
    , unsigned long* microsecond);

void bindFiledsMeta(MYSQL_RES* mysql_res, std::vector<FieldMeta>* fields_data);


/*
inline
boost::posix_time::ptime utcToLocal(time_t t)
{
    return boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(
        boost::posix_time::from_time_t(t));
}

inline
struct tm time_t_TO_tm(time_t t)
{
    return boost::posix_time::to_tm(utcToLocal(t));
}
*/


}
}

#endif
