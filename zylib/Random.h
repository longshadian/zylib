#pragma once

#include <ctime>
#include <random>
#include <algorithm>

namespace zylib {

extern std::default_random_engine g_engine;

void randomEngineInit();

template <typename T>
inline
T rand(T closed_begin, T closed_end)
{
    return std::uniform_int_distribution<T>(closed_begin, closed_end)(g_engine);
}

inline
int32_t rand()
{
    return std::uniform_int_distribution<int32_t>()(g_engine);
}

inline
int64_t rand64()
{
    return std::uniform_int_distribution<int64_t>()(g_engine);
}

template <class RandomAccessIterator>
inline
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last)
{
    std::shuffle(first, last, g_engine);
}

template <typename E>
class Random
{
public:
    Random() : m_engine(static_cast<unsigned int>(std::time(nullptr))) { }

    ~Random() = default;
    Random(const Random& rhs) = default;
    Random& operator=(const Random& rhs) = default;
    Random(Random&& rhs) = default;
    Random& operator=(Random&& rhs) = default;

    template <typename T>
    T rand()
    {
        return std::uniform_int_distribution<T>()(m_engine);
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

    E& engine() { return m_engine; }
    const E& engine() const { return m_engine; }
private:
    E m_engine;
};

using DefaultRandom = Random<std::default_random_engine>;

/*
template <class RandomAccessIterator>
inline
void random_shuffle(RandomAccessIterator first, RandomAccessIterator last)
{
    typename std::iterator_traits<RandomAccessIterator>::difference_type n = (last - first);
    if (n <= 0)
        return;
    while (--n) {
        auto k = rand()%(n+1);
        if (n != k)
            std::swap(first[n], first[k]);
    }
}
*/

//////////////////////////////////////////////////////////////////////////
}