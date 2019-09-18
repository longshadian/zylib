#include "zylib/uboost/BoostPTime.h"

#include <sstream>

namespace zylib {

std::string FormatPTime(const boost::posix_time::ptime& t, const char* fmt)
{
	try {
		std::ostringstream ostm{};
		if (fmt) {
			boost::posix_time::time_facet* facet = new boost::posix_time::time_facet(fmt);
			ostm.imbue(std::locale(std::locale(), facet));
		}
		ostm << t;
		return ostm.str();
	}
	catch (...) {
		return {};
	}
}

std::string FormatPTime_YYYYMMDD_HHMMSS(const boost::posix_time::ptime& t)
{
	const char* fmt = "%Y-%m-%d %H:%M:%S";
	return FormatPTime(t, fmt);
}

boost::posix_time::ptime BoostUniversalTime()
{
	return boost::posix_time::microsec_clock::universal_time();
}

boost::posix_time::ptime BoostLocalTime()
{
	return boost::posix_time::microsec_clock::local_time();
}


} // namespace zylib
