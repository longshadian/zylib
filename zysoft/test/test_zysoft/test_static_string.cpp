#include <zysoft/string/static_string.h>
#include "../doctest/doctest.h"

#include "TestDefine.h"
#include "test_log.h"


template <std::size_t N>
static void print_static_string(const zysoft::static_string<N>& s)
{
    std::cout << "c_str():          " << s.c_str() << "\n";
    std::cout << "capacity():       " << s.capacity() << "\n";
    std::cout << "empty():          " << s.empty() << "\n";
    std::cout << "szie():           " << s.size() << "\n";
    std::cout << "length():         " << s.length() << "\n";
    std::cout << "max_size():       " << s.max_size() << "\n";
    std::cout << "back():           " << s.back() << "\n";
    std::cout << "front():          " << s.front() << "\n";
}

static void Test_Print()
{
    zysoft::static_string<3> s;
    print_static_string(s);
}



#if 1
TEST_CASE("test static_string")
{
    PrintInfo("test static_string");
    try {
        Test_Print();
    } catch (const std::exception& e) {
        std::cout << e.what() << "\n";
        CHECK(false);
    }
}
#endif 

