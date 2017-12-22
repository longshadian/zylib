#pragma once

#include <mysql.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#include "mysqlcpp/Types.h"

namespace mysqlcpp {

class DateTime;

namespace util {

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
