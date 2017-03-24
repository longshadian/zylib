#pragma once

#include <cstdint>
#include <utility>

struct ConnID
{
    explicit ConnID(int32_t id = 0)
        : m_conn_id(id)
    {
    }

    operator bool() const
    {
        return m_conn_id != 0;
    }

    int32_t m_conn_id;
};

inline
bool operator==(const ConnID& lhs, const ConnID& rhs) 
{
    return lhs.m_conn_id == rhs.m_conn_id;
}

namespace std
{
    template<> struct hash<ConnID>
    {
        typedef ConnID argument_type;
        typedef std::size_t result_type;
        result_type operator()(const argument_type& conn_id) const
        {
            return std::hash<decltype(conn_id.m_conn_id)>()(conn_id.m_conn_id);
        }
    };
}
