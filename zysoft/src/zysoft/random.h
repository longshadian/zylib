#pragma once

#include <ctime>
#include <random>
#include <algorithm>

namespace zysoft
{

template <typename E>
class random
{
public:
    random() 
        : m_engine(std::random_device{}())
    {
    }

    ~random() = default;
    random(const random& rhs) = default;
    random& operator=(const random& rhs) = default;
    random(random&& rhs) = default;
    random& operator=(random&& rhs) = default;

    template <typename T>
    T rand()
    {
        return std::uniform_int_distribution<T>()(m_engine);
    }

    template <typename T>
    T rand(T closed_end)
    {
        return std::uniform_int_distribution<T>(T{}, closed_end)(m_engine);
    }

    template <typename T>
    T rand(T closed_begin, T closed_end)
    {
        return std::uniform_int_distribution<T>(closed_begin, closed_end)(m_engine);
    }

    template <class RandomAccessIterator>
    void shuffle(RandomAccessIterator first, RandomAccessIterator last)
    {
        std::shuffle(first, last, m_engine);
    }

    std::string gen_rand_str(std::size_t size) 
    {
        static const std::string VALID_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string str;
        str.reserve(size);
        std::generate_n(std::back_inserter(str), size, [&]() { return VALID_CHARS[rand(VALID_CHARS.size() - 1)]; });
        return str;
    }

    void gen_rand_data(void* data, std::size_t size)
    {
        std::uint8_t* p = reinterpret_cast<std::uint8_t*>(data);
        for (std::size_t i = 0; i != size; ++i) {
            p[i] = rand(0, 0xFF);
        }
    }

    E& internal_engine() { return m_engine; }
    const E& internal_engine() const { return m_engine; }
private:
    E m_engine;
};

using default_random = random<std::default_random_engine>;

} // namepsace zysoft


