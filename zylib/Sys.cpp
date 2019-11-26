#include "zylib/Sys.h"

#if defined(_WIN32)
#include <stdio.h>
#endif // _WIN32

namespace zylib {

void Fopen(std::FILE** f, const char* fname, const char* mod)
{
#if defined(_WIN32)
    fopen_s(f, fname, mod);
#else
    std::FILE* ff = std::fopen(fname, mod);
    if (ff) {
        *f = ff;
    }
#endif
}

std::FILE* Fopen(const char* fname, const char* mod)
{
    std::FILE* f = nullptr;
    Fopen(&f, fname, mod);
    return f;
}

} // namespace zylib
