#include "TestDefine.h"

#include <cstdio>
#include <cassert>

int TestMD5()
{
    if (0) {
        zylib::PerformanceTimer pt{};
        const char* fname = "C:/a.zip";
        std::FILE* f = std::fopen(fname, "rb");
        assert(f);

        zylib::MD5Context ctx{};
        zylib::MD5_Init(&ctx);

        std::array<char, 1024 * 16> arr{};
        while (1) {
            std::size_t n = std::fread(arr.data(), 1, arr.size(), f);
            if (n == 0)
                break;
            zylib::MD5_Update(&ctx, arr.data(), n);
        }
        std::fclose(f);

        std::array<std::uint8_t, 16> signature{};
        zylib::MD5_Final(&signature, &ctx);
        std::string md5_str = zylib::ToHex(signature.data(), signature.size());
        md5_str = zylib::ToUpperCase(md5_str);
        pt.Stop();
        DPrintf("file path: %s md5: %s cost: %d sec", fname, md5_str.c_str(), (std::int32_t)pt.CostSeconds());
    }

    return 0;
}
