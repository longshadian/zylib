#pragma once

#include <cstring>
#include <vector>
#include <list>
#include <type_traits>

template <typename T>
struct VarArray
{
    enum { SENTINEL_SIZE = sizeof(int32_t) };
    enum { VALUE_SIZE = sizeof(T) };

    static_assert(std::is_pod<T>::value,    "T mush be POD!");
    static_assert(VALUE_SIZE > 0,           "sizeof(T) mush > 0!");

    VarArray() : m_null_sentinel(), m_data() {}
    ~VarArray() {}
    VarArray(const VarArray& rhs) = delete;
    VarArray& operator=(const VarArray& rhs) = delete;

    size_t bytes() const
    {
        return SENTINEL_SIZE + VALUE_SIZE * m_data.size();
    }

    void serializeToBinary(std::vector<uint8_t>* buffer) const
    {
        buffer->resize(bytes());
        auto* pos = buffer->data();
        std::memcpy(pos, &m_null_sentinel, SENTINEL_SIZE);
        pos += SENTINEL_SIZE;
        if (m_data.empty())
            return;
        std::memcpy(pos, m_data.data(), VALUE_SIZE * m_data.size());
    }

    bool parseFromBinary(const uint8_t* pos, size_t len)
    {
        if (len < SENTINEL_SIZE)
            return false;
        auto data_len = len - SENTINEL_SIZE;
        if ((data_len % VALUE_SIZE) != 0)
            return false;
        auto cnt = data_len / VALUE_SIZE;

        std::memcpy(&m_null_sentinel, pos, SENTINEL_SIZE);
        pos += SENTINEL_SIZE;

        if (cnt != 0) {
            m_data.resize(cnt);
            std::memcpy(m_data.data(), pos, data_len);
        }
        return true;
    }

    int32_t        m_null_sentinel; // 空值哨兵，确保数据为空时，序列化后大小不等于0，至少>=4
    std::vector<T> m_data;
};

template <typename T>
struct VarList
{
    enum { SENTINEL_SIZE = sizeof(int32_t) };
    enum { VALUE_SIZE = sizeof(T) };

    static_assert(std::is_pod<T>::value,    "T mush be POD!");
    static_assert(VALUE_SIZE > 0,           "sizeof(T) mush > 0!");

    VarList() : m_null_sentinel(), m_data() {}
    ~VarList() {}
    VarList(const VarList& rhs) = delete;
    VarList& operator=(const VarList& rhs) = delete;

    size_t bytes() const
    {
        return SENTINEL_SIZE + VALUE_SIZE * m_data.size();
    }

    void serializeToBinary(std::vector<uint8_t>* buffer) const
    {
        buffer->resize(bytes());
        auto* pos = buffer->data();
        std::memcpy(pos, &m_null_sentinel, SENTINEL_SIZE);
        pos += SENTINEL_SIZE;
        if (m_data.empty())
            return;

        for (const auto& slot : m_data) {
            std::memcpy(pos, &slot, VALUE_SIZE);
            pos += VALUE_SIZE;
        }
    }

    bool parseFromBinary(const uint8_t* pos, size_t len)
    {
        if (len < SENTINEL_SIZE)
            return false;
        auto data_len = len - SENTINEL_SIZE;
        if ((data_len % VALUE_SIZE) != 0)
            return false;
        auto cnt = data_len / VALUE_SIZE;

        std::memcpy(&m_null_sentinel, pos, SENTINEL_SIZE);
        pos += SENTINEL_SIZE;

        while (cnt > 0) {
            --cnt;
            T slot;
            std::memcpy(&slot, pos, VALUE_SIZE);
            pos += VALUE_SIZE;
            m_data.emplace_back(slot);
        }
        return true;
    }

    int32_t         m_null_sentinel; // 空值哨兵，确保数据为空时，序列化后大小不等于0，至少>=4
    std::list<T>    m_data;
};
