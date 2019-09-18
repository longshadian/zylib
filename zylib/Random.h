#pragma once

#include <ctime>
#include <random>
#include <algorithm>

namespace zylib {

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
    T Rand()
    {
        return std::uniform_int_distribution<T>()(m_engine);
    }

    template <typename T>
    T Rand(T closed_begin, T closed_end)
    {
        return std::uniform_int_distribution<T>(closed_begin, closed_end)(m_engine);
    }

    template <class RandomAccessIterator>
    void Shuffle(RandomAccessIterator first, RandomAccessIterator last)
    {
        std::shuffle(first, last, m_engine);
    }

    E& Engine() { return m_engine; }
    const E& Engine() const { return m_engine; }
private:
    E m_engine;
};

using DefaultRandom = Random<std::default_random_engine>;

//////////////////////////////////////////////////////////////////////////
}