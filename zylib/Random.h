#pragma once

#include <random>
#include <algorithm>
#include <ctime>

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