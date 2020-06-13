#include <zysoft/string/convert_functions.h>
#include "../doctest/doctest.h"

#include "TestDefine.h"
#include "test_log.h"

static void Test_1()
{
    PrintInfo("test string convert functions");
    {
        const char* s1  = " 123";
        std::error_code ec{};
        LogInfo <<(int)zysoft::convert<char>::to_number(s1, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";

        ec.clear();
        LogInfo <<(int)zysoft::convert<char, zysoft::skip_blank<false>>::to_number(s1, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";
        CHECK(zysoft::convert<char, zysoft::skip_blank<false>>::to_number_default(s1, 11) == 11);

        std::string s2 = "65536";
        ec.clear();
        LogInfo << zysoft::convert<std::uint16_t>::to_number(s2, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";
        CHECK(zysoft::convert<std::uint16_t>::to_number_default(s2, 999) == 999);

        std::string s3 = "";
        ec.clear();
        LogInfo << zysoft::convert<int>::to_number(s3, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";
        CHECK(zysoft::convert<int>::to_number_default(s3, -1) == -1);

        std::string s4 = "0123456789";
        ec.clear();
        LogInfo << zysoft::convert<int>::to_number(s4, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";
        //CHECK(zysoft::convert<int>::to_number_default(s4, -1) == -1);
    }
}

static void Test_2()
{
    PrintInfo("test string convert functions float double long double");
    {
        const char* s1  = " 3.1415926";
        std::error_code ec{};
        LogInfo <<zysoft::convert<float>::to_number(s1, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";

        ec.clear();
        LogInfo <<zysoft::convert<float, zysoft::skip_blank<false>>::to_number(s1, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";
        //CHECK(zysoft::convert<char, zysoft::skip_blank<false>>::to_number_default(s1, 11) == 11);

        std::string s2 = "-65536.123";
        ec.clear();
        LogInfo << zysoft::convert<float>::to_number(s2, ec);
        LogInfo << ec.value() << " " << ec.message() << "\n";
    }
}

#if 0
TEST_CASE("test string convert functions")
{
    PrintInfo("test string convert functions");
    try {
        //Test_1();
        Test_2();
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        CHECK(false);
    }
}
#endif 
