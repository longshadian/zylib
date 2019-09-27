#pragma once

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
