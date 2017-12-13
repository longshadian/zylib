#pragma once

#include <array>

namespace knet {
namespace detail {

template <size_t L>
class FixedString
{
public:
    using ClassType = FixedString;
    enum { MAX_LEN = L };

    FixedString() : m_data() { }
    ~FixedString() {}
    FixedString(const FixedString& rhs) 
        : m_data(rhs.m_data) 
    {
    }

    FixedString& operator=(const FixedString& rhs)
    {
        if (this != &rhs) {
            m_data = rhs.m_data;
        }
        return *this;
    }

    FixedString(FixedString&& rhs) 
        : m_data(std::move(rhs.m_data))
    {
    }

    FixedString& operator=(FixedString&& rhs)
    {
        if (this != &rhs) {
            std::swap(m_data, rhs.m_data);
        }
        return *this;
    }

    const char* GetCString() const
    {
        return m_data.data();
    }

    bool SetString(const std::string& s)
    {
        return SetString(s.c_str(), s.length());
    }

    bool SetString(const char* s)
    {
        return SetString(s, std::strlen(s));
    }

    bool SetString(const char* s, size_t len)
    {
        if (len > MAX_LEN)
            return false;
        m_data.fill(0);
        std::memcpy(m_data.data(), s, len);
        return true;
    }

    size_t Length() const
    {
        return std::strlen(GetCString());
    }

    std::vector<uint8_t> GetBinary() const
    {
        if (MAX_LEN == 0)
            return {};
        std::vector<uint8_t> buffer{};
        buffer.resize(MAX_LEN);
        std::memcpy(buffer.data(), m_data.data(), MAX_LEN);
        return buffer;
    }

    bool SetBinary(const std::vector<uint8_t>& buffer)
    {
        if (buffer.size() > MAX_LEN)
            return false;
        m_data.fill(0);
        if (buffer.empty())
            return true;
        std::memcpy(m_data.data(), buffer.data(), buffer.size());
        return true;
    }

    bool operator==(const ClassType& rhs) const
    {
        return m_data == rhs.m_data;
    }

    bool operator!=(const ClassType& rhs) const
    {
        return !(*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& ostm, const ClassType& rhs)
    {
        ostm << rhs.GetCString();
        return ostm;
    }

private:
    std::array<char, L + 1> m_data;
};

} // detail
} // knet

