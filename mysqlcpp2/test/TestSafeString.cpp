#include <iostream>

#include <cstdlib>
#include <cstring>
#include <limits>

#include "mysqlcpp/detail/Convert.h"
#include "mysqlcpp/detail/SafeString.h"

#define BOOST_TEST_MODULE MyFirstTest
//#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp>

void printSafeString(const mysqlcpp::detail::SafeString& sf)
{
    std::cout << "length   " << sf.Length() << "\n";
    std::cout << "bin size " << sf.AsBinary().size() << "\n";
    std::cout << "cstring  " << sf.AsStringView() << "\n";
    std::cout << "===================\n";
}

BOOST_AUTO_TEST_CASE(TestSafeString)
{
    int i = 1;
    BOOST_TEST(i);
    BOOST_TEST(i != 2);
}

/*
int main()
{
    std::string s = "12345678";

    ::mysqlcpp::detail::SafeString sf{};
    printSafeString(sf);

    sf.Resize(s.size());
    std::memcpy(sf.GetPtr(), s.c_str(), s.size());
    printSafeString(sf);

    {
        std::cout << "start move\n";
        ::mysqlcpp::detail::SafeString sf_ex = std::move(sf);
        printSafeString(sf);
        printSafeString(sf_ex);
    }

    {
        std::cout << "start copy\n";
        sf.Clear();
        sf.Resize(s.size());
        std::memcpy(sf.GetPtr(), s.c_str(), s.size());
        printSafeString(sf);

        ::mysqlcpp::detail::SafeString sf_ex = sf;
        printSafeString(sf);
        printSafeString(sf_ex);
    }

    return 0;
}
*/
