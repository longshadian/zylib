#pragma once

#include <cstdint>
#include <ctime>
#include <cstring>
#include <string>

class Win32Utilities
{
public:
    static struct tm* LocaltimeEx(const time_t* t, struct tm* output);
    static std::string LocaltimeYYYMMDD_HHMMSS(std::time_t t);

    static std::string GetExePath();

    // 通过注册表获取进程PID
    static DWORD GetRegistryValueByKey(const char* path, const char* key);

private:
    static std::string StringFormat(const char *fmt, ...);
};
