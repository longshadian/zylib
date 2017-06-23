#include <array>
#include <iostream>
#include <cstring>

const int32_t MAGIC_NUM = 0xB18A2031;

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

void encrypt(const uint8_t* src, size_t len, uint32_t key, uint8_t* out)
{
    key ^= MAGIC_NUM;
    uint8_t arr[4];
    arr[0] = static_cast<uint8_t>(key & 0x000000FF);
    arr[1] = static_cast<uint8_t>((key & 0x0000FF00) >> 8);
    arr[2] = static_cast<uint8_t>((key & 0x00FF0000) >> 16);
    arr[3] = static_cast<uint8_t>((key & 0xFF000000) >> 24);
    for (size_t i = 0; i != len; ++i) {
        uint8_t val = src[i];
        val = static_cast<uint8_t>(~val) ^ arr[i % 4];
        encryptShift(&val);
        *out = val;
        ++out;
    }
}

void decrypt(const uint8_t* src, size_t len, uint32_t key, uint8_t* out)
{
    key ^= MAGIC_NUM;
    uint8_t arr[4];
    arr[0] = static_cast<uint8_t>(key & 0x000000FF);
    arr[1] = static_cast<uint8_t>((key & 0x0000FF00) >> 8);
    arr[2] = static_cast<uint8_t>((key & 0x00FF0000) >> 16);
    arr[3] = static_cast<uint8_t>((key & 0xFF000000) >> 24);
    for (size_t i = 0; i != len; ++i) {
        uint8_t val = src[i];
        decryptShift(&val);
        val ^= arr[i % 4];
        *out = static_cast<uint8_t>(~val);
        ++out;
    }
}

uint32_t decryptEx(uint32_t uid, uint32_t key)
{
    //uid ^= key;
    std::array<uint8_t, sizeof(uid)> src{};
    std::array<uint8_t, sizeof(uid)> dest{};
    std::memcpy(src.data(), &uid, src.size());
    for (size_t i = 0; i != src.size(); ++i) {
        uint8_t val = src[i];
        decryptShift(&val);
        dest[i] = static_cast<uint8_t>(~val);
    }

    uint32_t value = 0;
    std::memcpy(&value, dest.data(), dest.size());
    return value ^ key;
}

uint32_t encryptEx(uint32_t value, uint32_t key)
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

    uint32_t uid = 0;
    std::memcpy(&uid, dest.data(), dest.size());
    return uid;
}

int main()
{
    uint64_t uid = 123456;
    uint64_t uid_e = encryptEx(uid, MAGIC_NUM);
    uint64_t uid_d = decryptEx(uid_e, MAGIC_NUM);

    std::cout << "uid   " << uid << "\n";
    std::cout << "uid_e " << uid_e << "\n";
    std::cout << "uid_d " << uid_d << "\n";

    std::cout << (uid == uid_d) << "\n";
    return 0;
}
