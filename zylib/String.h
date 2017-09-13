#pragma once

#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <array>

namespace zylib {
namespace detail {

inline
std::string concatDetail(std::ostringstream* ostm)
{
    return ostm->str();
}

template <typename T, typename... Args>
inline
std::string concatDetail(std::ostringstream* ostm, T&& t, Args&&... arg)
{
    (*ostm) << std::forward<T>(t);
    return concatDetail(ostm, std::forward<Args>(arg)...);
}

} /// detail


template <typename... Args>
inline
std::string concat(Args&&... arg)
{
    std::ostringstream ostm;
    return detail::concatDetail(&ostm, std::forward<Args&&>(arg)...);
}

template <size_t L>
struct ArrayString
{
    enum { MAX_LEN = L };
    std::array<char, L + 1> m_data;

    const char* getString() const
    {
        return m_data.data();
    }

    bool setString(const std::string& s)
    {
        return setString(s.c_str(), s.length());
    }

    bool setString(const char* s)
    {
        return setString(s, std::strlen(s));
    }

    bool setString(const char* s, size_t len)
    {
        if (len > MAX_LEN)
            return false;
        m_data.fill(0);
        std::memcpy(m_data.data(), s, len);
        return true;
    }

    size_t length() const
    {
        return std::strlen(getString());
    }

    std::vector<uint8_t> getBinary() const
    {
        if (MAX_LEN == 0)
            return {};
        std::vector<uint8_t> buffer{};
        buffer.resize(MAX_LEN);
        std::memcpy(buffer.data(), m_data.data(), MAX_LEN);
        return buffer;
    }

    bool setBinary(const std::vector<uint8_t>& buffer)
    {
        if (buffer.size() > MAX_LEN)
            return false;
        m_data.fill(0);
        if (buffer.empty())
            return true;
        std::memcpy(m_data.data(), buffer.data(), buffer.size());
        return true;
    }

    bool operator==(const ArrayString& rhs) const
    {
        return m_data == rhs.m_data;
    }

    bool operator!=(const ArrayString& rhs) const
    {
        return !(*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& ostm, const ArrayString& rhs)
    {
        ostm << rhs.getString();
        return ostm;
    }
};

static_assert(std::is_pod<ArrayString<0>>::value, "ArrayString must be POD!");


} /// zylib
