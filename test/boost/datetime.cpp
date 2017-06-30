#include <boost/date_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>

boost::posix_time::ptime utcToLocal(time_t t)
{
    return boost::date_time::c_local_adjustor<boost::posix_time::ptime>::utc_to_local(
        boost::posix_time::from_time_t(t));
}

std::string formatPTime(boost::posix_time::ptime t, const char* fmt = nullptr)
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
        return{};
    }
}

void testSaveDisk()
{
    auto t = std::time(nullptr);
    auto local_t = utcToLocal(t);
    std::cout << "local_t " << formatPTime(local_t) << "\n";
    std::cout << local_t << "\n";
}

void testDay()
{
    //auto p1 = boost::posix_time::time_from_string("2017-06-28 00:00:00");
    //auto p2 = boost::posix_time::time_from_string("2017-06-28 23:59:59");
    //auto p2 = boost::posix_time::time_from_string("2017-06-29 30:00:00");
    auto p1 = boost::posix_time::from_time_t(-12);
    auto p2 = boost::posix_time::from_time_t(-12);
    std::cout << p1.date() << "\t" << p2.date() << "\t";
    std::cout << (p1.date() == p2.date()) << "\n";
}

int main()
{
    testDay();
    return 0;
}
