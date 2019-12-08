#include <cassert>
#include <ctime>
#include "zylib/zylib.h"
#include "TestDefine.h"

static zylib::DefaultRandom g_random;
int TestMisc()
{
    zylib::DefaultRandom dr{};
    if (1) {
        auto cur_t = std::time(nullptr);
        DPrintf("Localtime_HHMMSS:              %s", zylib::Localtime_HHMMSS(&cur_t).c_str());
        DPrintf("Localtime_HHMMSS_F:            %s", zylib::Localtime_HHMMSS_F().c_str());
        DPrintf("Localtime_YYYYMMDD_HHMMSS:     %s", zylib::Localtime_YYYYMMDD_HHMMSS(&cur_t).c_str());
        DPrintf("Localtime_YYYYMMDD_HHMMSS_F:   %s", zylib::Localtime_YYYYMMDD_HHMMSS_F().c_str());

        DPrintf("UTC_HHMMSS:                    %s", zylib::UTC_HHMMSS(&cur_t).c_str());
        DPrintf("UTC_HHMMSS_F:                  %s", zylib::UTC_HHMMSS_F().c_str());
        DPrintf("UTC_YYYYMMDD_HHMMSS:           %s", zylib::UTC_YYYYMMDD_HHMMSS(&cur_t).c_str());
        DPrintf("UTC_YYYYMMDD_HHMMSS_F:         %s", zylib::UTC_YYYYMMDD_HHMMSS_F().c_str());
    }

    if (1) {
        auto str = g_random.GenRandomString(50);
        DPrintf("ToUpperCase: %s", zylib::ToUpperCase(str).c_str());
        DPrintf("ToLowerCase: %s", zylib::ToLowerCase(str).c_str());
    }

    if (1) {
        //std::string src = "1234567890";
        std::string src = dr.GenRandomString(1024 * 140);
        std::string s;
        zylib::Base64Encode(src.data(), src.length(), &s);
        //DPrintf("Base64Encode: len: %d %s", (int)s.size(), s.c_str());

        std::size_t n2 = zylib::Base64DecodedLength(s.size());
        std::vector<std::uint8_t> buffer2{};
        buffer2.resize(n2);
        std::int32_t code = zylib::Base64Decode(s.data(), s.size(), buffer2.data(), &n2);

        std::string s2(buffer2.begin(), buffer2.begin() + n2);
        //DPrintf("Base64Decode: code: %d %s  is same: %d", code, s2.c_str(), (int)(src == s2));
        DPrintf("Base64Decode: code: %d is same: %d", code, (int)(src == s2));
    }

    if (1) {
        std::size_t N = 1024 * 1024;
        std::vector<std::uint8_t> src{};
        src.resize(N);

        for (int i = 0; i != 1024; ++i) {
            dr.GenRandomData(src.data(), src.size());

            std::string s1;
            std::string s2;
            {
                zylib::Base64Encode(src.data(), src.size(), &s1);

                std::size_t n = zylib::Base64DecodedLength(s1.size());
                s2.resize(n);
                std::int32_t code = zylib::Base64Decode(s1.data(), s1.size(), s2.data(), &n);
                s2.resize(n);
            }

            std::string sx1 = zylib::base64::Encode(src.data(), src.size());
            std::string sx2 = zylib::base64::Decode(sx1);

            assert(IsEqual(src.data(), src.size(), s2.data(), s2.size()));
            assert(IsEqual(src.data(), src.size(), sx2.data(), sx2.size()));
            assert(s1 == sx1);
            DPrintf("Base64Decode: is same: %d", i);
        }
    }

    return 0;
}

