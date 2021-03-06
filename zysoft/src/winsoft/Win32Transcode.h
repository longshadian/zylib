#pragma once

#include <string>

#include <Windows.h>
#include <mbstring.h>

namespace winsoft
{

#pragma warning(disable: 4127)

#define CP_GBK      (936)       // 简体中文code page

#define CHECK_INPUT()  do { if ( nullptr == in || 0 == len /*|| '\0' == in[0]字符串带0是允许的*/ ) { out.erase(); return; } } while(0)

inline void Unicode_to_UTF8(const wchar_t* in, size_t len, std::string& out)
{
    CHECK_INPUT();
    size_t out_len = len * 3 + 1;
    char* pBuf = new char[out_len];
    if (!pBuf) {
        return;
    }

    char* pResult = pBuf;
    memset(pBuf, 0, out_len);

    out_len = ::WideCharToMultiByte(CP_UTF8, 0, in, (int)len, pBuf, (int)len * 3, nullptr, nullptr);
    out.assign(pResult, out_len);

    delete[] pResult;
}

inline void UTF8_to_Unicode(const char* in, size_t len, std::wstring& out)
{
    CHECK_INPUT();
    wchar_t* pBuf = new wchar_t[len + 1];
    if (!pBuf) {
        return;
    }

    memset(pBuf, 0, (len + 1) * sizeof(wchar_t));
    wchar_t* pResult = pBuf;

    int out_len;

    out_len = ::MultiByteToWideChar(CP_UTF8, 0, in, (int)(intptr_t)len, pBuf, (int)len * sizeof(wchar_t));
    out.assign(pResult, out_len);

    delete[] pResult;
}

inline void ANSI_to_Unicode(const char* in, size_t len, std::wstring& out)
{
    CHECK_INPUT();

    //assert( _mbslen((const unsigned char*)in) == strlen(in) ); // ANSI 要求必须是纯英文
    int wbufferlen = (int)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, in, (int)len, nullptr, 0);
    wchar_t* pwbuffer = new wchar_t[wbufferlen + 4];
    if (!pwbuffer) {
        return;
    }
    wbufferlen = (int)::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, in, (int)len, pwbuffer, wbufferlen + 2);
    pwbuffer[wbufferlen] = '\0';
    out.assign(pwbuffer, wbufferlen);
    delete[] pwbuffer;
}

inline void Unicode_to_ANSI(const wchar_t* in, size_t len, std::string& out)
{
    CHECK_INPUT();

    int bufferlen = (int)::WideCharToMultiByte(CP_ACP, 0, in, (int)len, nullptr, 0, nullptr, nullptr);
    char* pBuffer = new char[bufferlen + 4];
    if (!pBuffer) {
        return;
    }
    int out_len = ::WideCharToMultiByte(CP_ACP, 0, in, (int)len, pBuffer, bufferlen + 2, nullptr, nullptr);
    pBuffer[bufferlen] = '\0';
    out.assign(pBuffer, out_len);
    delete[] pBuffer;
    //assert( _mbslen((const unsigned char*)out.c_str()) == strlen(out.c_str()) );
}

inline void Unicode_to_GBK(const wchar_t* in, size_t len, std::string& out)
{
    CHECK_INPUT();

    int bufferlen = (int)::WideCharToMultiByte(CP_GBK, 0, in, (int)len, nullptr, 0, nullptr, nullptr);
    char* pBuffer = new char[bufferlen + 4];
    if (!pBuffer) {
        return;
    }
    int out_len = ::WideCharToMultiByte(CP_GBK, 0, in, (int)len, pBuffer, bufferlen + 2, nullptr, nullptr);
    pBuffer[bufferlen] = '\0';
    out.assign(pBuffer, out_len);
    delete[] pBuffer;
}

inline void GBK_to_Unicode(const char* in, size_t len, std::wstring& out)
{
    CHECK_INPUT();
    int wbufferlen = (int)::MultiByteToWideChar(CP_GBK, MB_PRECOMPOSED, in, (int)len, nullptr, 0);
    wchar_t* pwbuffer = new wchar_t[wbufferlen + 4];
    if (!pwbuffer) {
        return;
    }
    wbufferlen = (int)::MultiByteToWideChar(CP_GBK, MB_PRECOMPOSED, in, (int)len, pwbuffer, wbufferlen + 2);
    pwbuffer[wbufferlen] = '\0';
    out.assign(pwbuffer, wbufferlen);
    delete[] pwbuffer;
}

inline void ANSI_to_UTF8(const char* in, size_t len, std::string& out)
{
    std::wstring wStr;
    ANSI_to_Unicode(in, len, wStr);
    Unicode_to_UTF8(wStr.c_str(), wStr.length(), out);
}

inline void UTF8_to_ANSI(const char* in, size_t len, std::string& out)
{
    std::wstring wstrWrite;
    UTF8_to_Unicode(in, len, wstrWrite);
    Unicode_to_ANSI(wstrWrite.c_str(), wstrWrite.length(), out);
}

inline std::string ANSI_to_UTF8(const char* in, size_t len)
{
    std::string dst;
    ANSI_to_UTF8(in, len, dst);
    return dst;
}

inline std::string UTF8_to_ANSI(const char* in, size_t len)
{
    std::string dst;
    UTF8_to_ANSI(in, len, dst);
    return dst;
}

inline std::string ANSI_to_UTF8(const std::string& in)
{
    std::string dst;
    ANSI_to_UTF8(in.data(), in.length(), dst);
    return dst;
}

inline std::string UTF8_to_ANSI(const std::string& in)
{
    std::string dst;
    UTF8_to_ANSI(in.data(), in.length(), dst);
    return dst;
}

}
