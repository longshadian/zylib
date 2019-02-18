#ifndef _GAME_TYPE_H_
#define _GAME_TYPE_H_

#include <memory>

//Эђ
constexpr
int64_t operator "" _w(unsigned long long v)
{
    return v * 10000;
}

//вк
constexpr
int64_t operator "" _ww(unsigned long long v)
{
    return v * 100000000;
}


#endif
