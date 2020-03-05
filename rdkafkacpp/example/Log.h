#pragma once

#include <cstdlib>

#define LOG_DEBUG(fmt, ...) do { fprintf(stdout, "[%4d] [DEBUG  ] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
#define LOG_WARN(fmt, ...) do { fprintf(stdout,  "[%4d] [WARNING] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
