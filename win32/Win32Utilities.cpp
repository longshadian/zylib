#include "stdafx.h"

#include <windows.h>
#include <TlHelp32.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <chrono>
#include <array>
#include <vector>

#include "Win32Utilities.h"

#define WindowsPrintLog(fmt, ...) if (0) printf(fmt "\n", ##__VA_ARGS__)

struct tm* Win32Utilities::LocaltimeEx(const time_t* t, struct tm* output)
{
#ifdef WIN32
    localtime_s(output, t);
#else
    localtime_r(t, output);
#endif
    return output;
}

std::string Win32Utilities::LocaltimeYYYMMDD_HHMMSS(std::time_t t)
{
    struct tm cur_tm{};
    LocaltimeEx(&t, &cur_tm);
    char buffer[128] = { 0 };

    snprintf(buffer, sizeof(buffer), "%04d%02d%02d-%02d%02d%02d"
        , cur_tm.tm_year + 1900, cur_tm.tm_mon+1, cur_tm.tm_mday
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
        );
    std::string s = buffer;
    return s;
}

std::string Win32Utilities::GetExePath()
{
	char szFullPath[MAX_PATH];
	char szdrive[_MAX_DRIVE];
	char szdir[_MAX_DIR];
	::GetModuleFileNameA(NULL, szFullPath, MAX_PATH);
	_splitpath_s(szFullPath, szdrive, _MAX_DRIVE, szdir, _MAX_DIR, NULL, NULL, NULL, NULL);

	std::string szPath;
	szPath = StringFormat("%s%s", szdrive, szdir);
	//szPath = szPath.Left(szPath.GetLength() - 1);

	return szPath;
}

DWORD Win32Utilities::GetRegistryValueByKey(const char* path, const char* key)
{
    HKEY hkey;
    DWORD szLocation = 0;
    DWORD dwSize = sizeof(DWORD);
    DWORD dwType = REG_DWORD;

    LPCTSTR billPath = (LPCTSTR)path;
    LSTATUS ret = ::RegOpenKeyEx(HKEY_CURRENT_USER, billPath, 0, KEY_READ, &hkey);
    if (ret != ERROR_SUCCESS) {
        return szLocation;
    }
    ret = ::RegQueryValueEx(hkey, (LPCTSTR)key, NULL, &dwType, (LPBYTE)&szLocation, &dwSize);
    if (ret != ERROR_SUCCESS) {
        szLocation = 0;
    }
    ::RegCloseKey(hkey);
    return szLocation;
}

//格式化字符串string
std::string Win32Utilities::StringFormat(const char *fmt, ...)
{
	std::string strResult = "";
	if (NULL != fmt)
	{
		va_list marker = NULL;
		va_start(marker, fmt);                            //初始化变量参数 
		size_t nLength = _vscprintf(fmt, marker) + 1;    //获取格式化字符串长度
		std::vector<char> vBuffer(nLength, '\0');        //创建用于存储格式化字符串的字符数组
		int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
		if (nWritten > 0)
		{
			strResult = &vBuffer[0];
		}
		va_end(marker);                                    //重置变量参数
	}
	return strResult;
}

