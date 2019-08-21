#include "stdafx.h"
#include "common/Code.h"

bool Code::IsUtf8(const std::string& str)
{
    return IsUtf8Internal(str.data(), str.length());
}

bool Code::IsUtf8(const char* str)
{
    return IsUtf8Internal(str, std::strlen(str));
}

bool Code::IsUtf8Internal(const char* pos, size_t len)
{
    size_t multi_len = 0;
    for (size_t i = 0; i != len; ++i) {
        unsigned c = static_cast<unsigned char>(pos[i]);
        if (multi_len == 0) {
            if (c <= 0x7F)          //0xxx xxxx     ascii码
                multi_len = 1;
            else if (c <= 0xDF)     //110x xxxx
                multi_len = 2;
            else if (c <= 0xEF)     //1110 xxxx
                multi_len = 3;
            else if (c <= 0xF7)     //1111 0xxx
                multi_len = 4;
            else
                return false;       //不考虑4字节以后的编码
            multi_len--;
        } else {
            if ((c & 0xC0) != 0x80) //最高2bit不是10xx xxxx
                return false;
            multi_len--;
        }
    }
    if (multi_len > 0)
        return false;
    return true;
}

