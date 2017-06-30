#include <array>
#include <iostream>
#include <cstring>
#include <ios>
#include <sstream>

const uint64_t MAGIC_NUM = 0x1A2B3C4D;


static uint8_t getHeight_3(uint8_t val)
{
    return uint8_t(((val) & 0xE0) >> 5);
}

static uint8_t getLower_5(uint8_t val)
{
    return uint8_t((val) & 0x1F);
}

static uint8_t	getHeight_5(uint8_t val)
{
    return uint8_t(((val) & 0xF8) >> 3);
}

static uint8_t	getLower_3(uint8_t val)
{
    return uint8_t((val) & 0x07);
}

static void encryptShift(uint8_t* val)
{
    uint8_t height = getHeight_3(*val);
    uint8_t lower = getLower_5(*val);
    *val = static_cast<uint8_t>(lower << 3) | height;
}

static void decryptShift(uint8_t* val)
{
    uint8_t height = getHeight_5(*val);
    uint8_t lower = getLower_3(*val);
    *val = static_cast<uint8_t>(lower << 5) | height;
}

uint64_t decryptEx(uint64_t uid, uint64_t key)
{
    std::array<uint8_t, sizeof(uid)> src{};
    std::array<uint8_t, sizeof(uid)> dest{};
    std::memcpy(src.data(), &uid, src.size());
    for (size_t i = 0; i != src.size(); ++i) {
        uint8_t val = src[i];
        decryptShift(&val);
        dest[i] = static_cast<uint8_t>(~val);
    }

    uint64_t value = 0;
    std::memcpy(&value, dest.data(), dest.size());
    return value ^ key;
    return value;
}

uint64_t encryptEx(uint64_t value, uint64_t key)
{
    value ^= key;
    std::array<uint8_t, sizeof(value)> src{};
    std::array<uint8_t, sizeof(value)> dest{};
    std::memcpy(src.data(), &value, src.size());
    for (size_t i = 0; i != src.size(); ++i) {
        uint8_t val = src[i];
        val = static_cast<uint8_t>(~val);
        encryptShift(&val);
        dest[i] = val;
    }

    uint64_t uid = 0;
    std::memcpy(&uid, dest.data(), dest.size());
    return uid;
}

int main()
{
    uint64_t uid = 123456;
    for (uint64_t i = 0; i != 20; ++i) {
        uid += i;
        uint64_t uid_e = encryptEx(uid, MAGIC_NUM);
        uint64_t uid_d = decryptEx(uid_e, MAGIC_NUM);
        printf("%lu %lu %lu\n", uid, uid_e, uid_d);
    }

    std::ostringstream ostm{};
    ostm << std::oct << (uid ^ MAGIC_NUM);
    std::cout << ostm.str() << "\n";

    ostm.str("");
    ostm << std::hex << encryptEx(uid, MAGIC_NUM);
    std::cout << ostm.str() << "\n";

    return 0;
}
