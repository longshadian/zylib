#pragma once

#include <cstring>  // std::memset
#include <cstdio>

#include <array>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <type_traits>

#include "zysoft/sys.h"

namespace zysoft
{

template< typename I
        , typename O
        >
inline void hex_dump(I b, I e, O o)
{
    static const std::uint8_t hex[] = "0123456789abcdef";
    while (b != e) {
        *o++ = hex[*b >> 4];
        *o++ = hex[*b++ & 0xf];
    }
}

template<typename T>
inline void bzero(T* t)
{
    static_assert(std::is_pod<T>::value, "T must be pod!");
    std::memset(t, 0, sizeof(T));
}

template <typename E>
inline std::underlying_type_t<E> EnumValue(E e)
{
    //static_assert(std::is_enum<E>::value, "E must be enum or enum class !");
    return static_cast<std::underlying_type_t<E>>(e);
}

inline std::string ToUpperCase(const std::string& src)
{
    if (src.empty())
        return "";
    std::string dst;
    dst.resize(src.size());
    std::transform(src.begin(), src.end(), dst.begin(), [](int c) { return static_cast<char>(::toupper(c)); });
    return dst;
}

inline std::string ToLowerCase(const std::string& src)
{
    if (src.empty())
        return "";
    std::string dst;
    dst.resize(src.size());
    std::transform(src.begin(), src.end(), dst.begin(), [](int c) { return static_cast<char>(::tolower(c)); });
    return dst;
}

inline std::string CatFile(const char* f)
{
    std::FILE* fp = fopen(f, "rb");
    if (!fp)
        return "";

    std::string content;
    std::array<char, 1024 * 16> buffer{};
    while (1) {
        std::size_t readn = std::fread(buffer.data(), 1, buffer.size(), fp);
        if (readn == 0)
            break;
        content.append(buffer.data(), buffer.data() + readn);
    }
    std::fclose(fp);
    return content;
}

inline bool CatFile(const std::string& path, std::string* out)
{
    std::FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
        return false;
    }

    bool succ = true;
    std::string content;
    std::array<char, 1024 * 16> buffer;
    std::uint64_t total = 0;
    while (true) {
        std::size_t readn = std::fread(buffer.data(), 1, buffer.size(), f);
        if (readn == 0) {
            break;
        }
        if (readn > buffer.size()) {
            succ = false;
            break;
        }
        total += readn;
        content.append(buffer.data(), buffer.data() + readn);
    }
    std::fclose(f);
    if (!succ)
        return succ;
    std::swap(*out, content);
    return true;
}
 
inline std::vector<std::string> StrSplit(const std::string& s, char c)
{
    std::vector<std::string> out;
    if (s.empty())
        return out;

    std::istringstream istm(s);
    std::string temp;
    while (std::getline(istm, temp, c)) {
        out.push_back(temp);
    }
    return out;
}

inline std::size_t StrReplace(std::string* str, char src, char dest)
{
    size_t t = 0;
    std::transform(str->begin(), str->end(), str->begin(),
        [&t, src, dest](char& c)
        {
            if (c == src) {
                c = dest;
                ++t;
            }
            return c;
        } );
    return t;
}

inline std::size_t StrRemove(std::string* str, char src)
{
    auto it = std::remove(str->begin(), str->end(), src);
    auto n = std::distance(it, str->end());
    str->erase(it, str->end());
    return n;
}

} // namespace zysoft

