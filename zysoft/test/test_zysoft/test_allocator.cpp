#include <zysoft/memory/null_allocator.h>
#include "../doctest/doctest.h"

#include "TestDefine.h"
#include "test_log.h"

TEST_CASE("test allocator")
{
    LogInfo("test allocator");
    {
        zysoft::null_allocator<std::string> alloc;
        alloc.allocate(111);
    }

}

