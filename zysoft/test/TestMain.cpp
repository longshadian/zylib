#include "TestDefine.h"

#include <cassert>


int main(int argc, char** argv)
{
#if defined(_WIN32)
    system("chcp 65001");
#endif

    assert(TestMisc()==0);
    assert(TestMD5()==0);

#if defined(_WIN32)
    system("pause");
#endif
    return 0;
}
