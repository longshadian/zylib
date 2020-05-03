#include "TestDefine.h"
#include "test_log.h"
#include <zysoft/memory/auto_buffer.h>

#include "../doctest/doctest.h"

#define member_v(a) (decltype(a))

TEST_CASE("test zysoft auto buffer")
{
    {
        zysoft::auto_buffer<std::int8_t, 10> ab{100};
        std::fill(ab.begin(), ab.end(), 0);
        CHECK(ab.size() == 100);

    }

    {
        // auto_buffer::resize()
        zysoft::auto_buffer<std::int8_t, 10> ab{1};
        CHECK(ab.resize(10));
    }

}
