#include "zysoft/random.h"

#include "../doctest/doctest.h"

TEST_CASE("test zysoft random")
{
    {
        zysoft::default_random dr;
        for (int i = 0; i != 100; ++i) {
            auto n = dr.rand<std::size_t>(1, 1024 * 10);
            std::string s = dr.gen_rand_str(n);
            CHECK(s.size() == n);
            std::string s1 = s;
            dr.shuffle(s1.begin(), s1.end());
            CHECK(s != s1);
        }
    }
}

