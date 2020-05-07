#pragma once

#include <string>
#include <cstddef>

namespace zysoft
{ 

inline const char* c_str_data(const std::string& s)
{
    return s.c_str();
}

inline const char* c_str_data(const char* s)
{
    return s;
}

inline const wchar_t* c_str_data(const std::wstring& s)
{
    return s.c_str();
}

inline const wchar_t* c_str_data(const wchar_t* s)
{
    return s;
}

inline std::size_t c_str_len(const std::string& s)
{
    return s.length();
}

inline std::size_t c_str_len(const char* s)
{
    return std::strlen(s);
}

inline std::size_t c_str_len(const std::wstring& s)
{
    return s.length();
}

} // namespace zysoft

