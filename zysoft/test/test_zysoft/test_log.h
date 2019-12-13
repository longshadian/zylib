#pragma once

#include <cstdio>

#define LogInfo(fmt, ...) printf("[INFO] [line:%d] " fmt "\n", __LINE__, ##__VA_ARGS__)
#define LogWarn(fmt, ...) printf("[WARN] [line:%d] " fmt "\n", __LINE__, ##__VA_ARGS__)

