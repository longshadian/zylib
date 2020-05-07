#pragma once

#include <cstdio>

#define LogInfo(fmt, ...) printf("[INFO] [%s] [line:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LogWarn(fmt, ...) printf("[WARN] [%s] [line:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

