#include <zysoft/string/split_functions.h>
#include "../doctest/doctest.h"

#include "TestDefine.h"
#include "test_log.h"

// TODO 测试宽字符

TEST_CASE("test string split functions")
{
    LogInfo("test string split functions");
    {
        std::string src = "192.168";
        std::string s1, s2;
        auto n = zysoft::split(src, '.', s1, s2);
        CHECK(n == 1);
        CHECK(s1 == "192");
        CHECK(s2 == "168");
    }

    {
        std::string s1, s2, s3;
        auto n = zysoft::split("1.2222.3", '.', s1, s2, s3);
        CHECK(n == 3);
        CHECK(s1 == "1");
        CHECK(s2 == "2222");
        CHECK(s3 == "3");
    }

    {
        std::string s1, s2, s3, s4;
        auto n = zysoft::split("192.168.-1000.1", '.', s1, s2, s3, s4);
        CHECK(n == 4);
        CHECK(s1 == "192");
        CHECK(s2 == "168");
        CHECK(s3 == "-1000");
        CHECK(s4 == "1");
    }

    {
        std::string s1, s2, s3, s4, s5;
        auto n = zysoft::split("192.168.-1000.1.abc123456", '.', s1, s2, s3, s4, s5);
        CHECK(n == 5);
        CHECK(s1 == "192");
        CHECK(s2 == "168");
        CHECK(s3 == "-1000");
        CHECK(s4 == "1");
        CHECK(s5 == "abc123456");
    }

    {
        std::string s1, s2, s3, s4, s5, s6;
        auto n = zysoft::split("2018-12-01-16-55-31", '-', s1, s2, s3, s4, s5, s6);
        CHECK(n == 6);
        CHECK(s1 == "2018");
        CHECK(s2 == "12");
        CHECK(s3 == "01");
        CHECK(s4 == "16");
        CHECK(s5 == "55");
        CHECK(s6 == "31");
    }
}

TEST_CASE("test string split functions incorrect")
{
    LogInfo("test string split functions incorrect");
    {
        std::string src = "192.168.123";
        std::string s1, s2;
        auto n = zysoft::split(src, '.', s1, s2);
        CHECK(n == 1);
        CHECK(s1 == "192");
        CHECK(s2 == "168.123");
    }

    {
        std::string s1, s2;
        auto n = zysoft::split("abc", '.', s1, s2);
        CHECK(n == 0);
        CHECK(s1 == "abc");
        CHECK(s2 == "");
    }

    {
        std::string s1, s2;
        auto n = zysoft::split("", '.', s1, s2);
        CHECK(n == 0);
        CHECK(s1 == "");
        CHECK(s2 == "");
    }

    {
        std::string s1, s2;
        auto n = zysoft::split(".abc", '.', s1, s2);
        CHECK(n == 1);
        CHECK(s1 == "");
        CHECK(s2 == "abc");
    }

    {
        std::string s1, s2;
        auto n = zysoft::split(".abc.123", '.', s1, s2);
        CHECK(n == 1);
        CHECK(s1 == "");
        CHECK(s2 == "abc.123");
    }

    {
        std::string s1, s2;
        auto n = zysoft::split("....a.bc.123...", '.', s1, s2);
        CHECK(n == 1);
        CHECK(s1 == "");
        CHECK(s2 == "...a.bc.123...");
    }

}

