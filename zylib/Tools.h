#pragma once

#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <sstream>

namespace zylib {

void Init();

std::vector<std::string> StringSplit(const std::string& s, char c);

//字符串替换,字符串str中的src字符替换成dest,返回替换个数
std::size_t StringReplace(std::string* str, char src, char dest);

template <class RandomAccessIterator>
void LinearRandomShuffle(RandomAccessIterator first, RandomAccessIterator last)
{
    typename std::iterator_traits<RandomAccessIterator>::difference_type n = (last - first);
    if (n <= 0)
        return;
    while (--n) {
        std::swap(first[n], first[rand() % (n + 1)]);
    }
}

template <typename T>
void BZzero(T* t)
{
    static_assert(std::is_pod<T>::value, "T must be pod!");
    std::memset(t, 0, sizeof(T));
}

std::string CatFile(const char* f);
bool CatFile(const std::string& path, std::string* out);

std::string ToUpperCase(const std::string& src);
std::string ToLowerCase(const std::string& src);
std::string ToHex(const void* data, std::size_t len);

struct tm* Localtime(const std::time_t* t, struct tm* output);
std::string LocaltimeYYYMMDD_HHMMSS(std::time_t t);


// little/big endian conversion
short	BigShort(short l);
short	LittleShort(short l);
int		BigLong(int l);
int		LittleLong(int l);
float	BigFloat(float l);
float	LittleFloat(float l);
void	BigRevBytes(void *bp, int elsize, int elcount);
void	LittleRevBytes(void *bp, int elsize, int elcount);
void	LittleBitField(void *bp, int elsize);


template <typename E>
static std::underlying_type_t<E> EnumValue(E e)
{
    //static_assert(std::is_enum<E>::value, "E must be enum or enum class !");
    return static_cast<std::underlying_type_t<E>>(e);
}

//////////////////////////////////////////////////////////////////////////
}


namespace zylib {
namespace detail {
void	Swap_Init();

} // namespace detail
} // namespace zylib
