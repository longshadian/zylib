#include <cassert>
#include <vector>


#include "TestDefine.h"
#include "test_auto_buffer.h"

int main(int argc, char** argv)
{
#if defined(_WIN32)
    system("chcp 65001");
#endif

    assert(TestMD5()==0);

    test_auto_buffer();

    std::vector<char> v;

#if defined(_WIN32)
    system("pause");
#endif
    return 0;
}
