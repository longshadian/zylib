#include <zysoft/memory/null_allocator.h>
#include "../doctest/doctest.h"

#include "TestDefine.h"
#include "test_log.h"

TEST_CASE("test allocator")
{
    PrintDebug("test allocator");
    {
        zysoft::null_allocator<std::string> alloc;
        //(void)alloc.allocate(111);
    }

}

