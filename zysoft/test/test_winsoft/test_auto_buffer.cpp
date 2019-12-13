#include "test_.h"

#include "TestDefine.h"
#include "test_log.h"
#include "zysoft/auto_buffer.h"

void test_transcode()
{
    zysoft::auto_buffer<std::int8_t, 10> ab{100};
    std::fill(ab.begin(), ab.end(), 0);

    LogInfo("array size %llu", ab.size());
}

