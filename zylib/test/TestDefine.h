#pragma once

#include <cstdio>
#include <ctime>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

#include "zylib/zylib.h"

#define APrintf(severity, fmt, ...) \
    do { \
        printf("[%s] [%s] [line:%04d] " fmt "\n", zylib::Localtime_HHMMSS_F().c_str(), severity, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define DPrintf(fmt, ...) APrintf("DEBUG  ", fmt, ##__VA_ARGS__)
#define WPrintf(fmt, ...) APrintf("WARNING", fmt, ##__VA_ARGS__)


int TestMisc();
int TestMD5();

bool IsEqual(const std::vector<std::uint8_t>& s1, const std::vector<std::uint8_t>& s2);
bool IsEqual(const void* s1, std::size_t s1_len, const void* s2, std::size_t s2_len);
