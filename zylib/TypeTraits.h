#pragma once

#include <ctime>
#include <sys/time.h>

#include <cmath>
#include <chrono>
#include <thread>
#include <type_traits>

#include <iostream>

namespace zylib {

template <int64_t T, uint32_t R>
struct Power
{
    enum { VALUE = T * Power<T, R - 1>::VALUE};
};

template <int64_t T>
struct Power<T, 0>
{
    enum {VALUE = 1};
};

} // zylib
