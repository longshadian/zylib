#pragma once

#include <cstdio>

namespace network
{

#define NETWORK_LOG

#ifdef NETWORK_LOG
 #define NETWORK_DPrintf(fmt, ...) do { printf("[%4d] [DEBUG  ] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
 #define NETWORK_WPrintf(fmt, ...) do { printf("[%4d] [WARNING] [%s] " fmt "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
#else
 #define NETWORK_DPrintf(fmt, ...) do { } while (0)
 #define NETWORK_WPrintf(fmt, ...) do { } while (0)
#endif

} // namespace network
