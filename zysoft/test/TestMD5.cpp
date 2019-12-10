#include "TestDefine.h"

#include <cstdio>
#include <cassert>

#include "zysoft/sys.h"

int TestMD5()
{
    if (0) {
        PerformanceTimer pt{};
        const char* fname = "C:/a.zip";
        std::FILE* f = zysoft::fopen(fname, "rb");
        assert(f);

        zysoft::MD5Context ctx{};
        zysoft::MD5_Init(&ctx);

        std::array<char, 1024 * 16> arr{};
        while (1) {
            std::size_t n = std::fread(arr.data(), 1, arr.size(), f);
            if (n == 0)
                break;
            zysoft::MD5_Update(&ctx, arr.data(), n);
        }
        std::fclose(f);

        std::array<std::uint8_t, 16> signature{};
        zysoft::MD5_Final(&signature, &ctx);

        std::vector<char> buffer{};
        buffer.resize(signature.size() * 2);
        zysoft::hex_dump(signature.cbegin(), signature.cend(), buffer.data());
        std::string md5_str{ buffer.begin(), buffer.end() };
        //std::string md5_str = zysoft::hex_dump(signature.data(), signature.size());
        //md5_str = zysoft::ToUpperCase(md5_str);
        pt.Stop();
        DPrintf("file path: %s md5: %s cost: %d sec", fname, md5_str.c_str(), (std::int32_t)pt.CostSeconds());
    }

    return 0;
}
