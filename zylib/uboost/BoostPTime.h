#pragma once

#include <string>
#include <ctime>

#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

namespace zylib {

inline
boost::posix_time::ptime UtcToLocal(std::time_t t)
{
    return boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(
        boost::posix_time::from_time_t(t));
}

std::string FormatPTime(const boost::posix_time::ptime& t, const char* fmt = nullptr);
std::string FormatPTime_YYYYMMDD_HHMMSS(const boost::posix_time::ptime& t);
boost::posix_time::ptime BoostUniversalTime();
boost::posix_time::ptime BoostLocalTime();

}
