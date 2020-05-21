#include <zysoft/string/case_functions.h>
#include "../doctest/doctest.h"

#include "TestDefine.h"
#include "test_log.h"

// TODO 测试宽字符

#if 0
TEST_CASE("test string case functions")
{
    PrintInfo("test string case functions");
    {
        std::string sl;
        std::string su;
        for (int i = 'a'; i <= 'z'; ++i) {
            sl.push_back(static_cast<char>(i));
        }

        for (int i = 'A'; i <= 'Z'; ++i) {
            su.push_back(static_cast<char>(i));
        }
            
        CHECK(sl == zysoft::to_lower(su));
        CHECK(su == zysoft::to_upper(sl));

        std::string sl_1(sl);
        std::string su_1(su);
        zysoft::make_upper(sl_1);
        zysoft::make_lower(su_1);
        CHECK(sl_1 == su);
        CHECK(su_1 == sl);
    }

    {
        // 不在A-Z a-z范围内
        std::string s;
        for (int i = 0; i <= 255; ++i) {
            s.push_back(static_cast<char>(i));
        }
        std::string sl = zysoft::to_lower(s);
        std::string su = zysoft::to_upper(s);

        for (int i = 0; i <= 255; ++i) {
            if (('A' <= i && i <= 'Z') || ('a' <= i && i <='z')) {
                char c = sl[i];
                CHECK(('a' <= c && c <= 'z'));
            } else {
                CHECK(s[i] == sl[i]);
            }
        }

        for (int i = 0; i <= 255; ++i) {
            if (('A' <= i && i <= 'Z') || ('a' <= i && i <='z')) {
                char c = su[i];
                CHECK(('A' <= c && c <= 'Z'));
            } else {
                CHECK(s[i] == su[i]);
            }
        }
    }
}
#endif

