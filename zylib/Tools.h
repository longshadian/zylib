#pragma once

#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#endif

namespace zylib {

void Init();

std::vector<std::string> StringSplit(const std::string& s, char c);

// 字符串替换,字符串str中的src字符替换成dest,返回替换个数
std::size_t StringReplace(std::string* str, char src, char dest);

// 字符串删除，
void StringRemove(std::string* str, char src);

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
std::string LocaltimeYYYYMMDD_HHMMSS(std::time_t t);


// little/big endian conversion
std::int16_t BigInt16(std::int16_t l);
std::int16_t LittleInt16(std::int16_t l);
std::int32_t BigInt32(std::int32_t l);
std::int32_t LittleInt32(std::int32_t l);
std::int64_t BigInt64(std::int64_t l);
std::int64_t LittleInt64(std::int64_t l);

std::uint16_t BigUInt16(std::uint16_t l);
std::uint16_t LittleUInt16(std::uint16_t l);
std::uint32_t BigUInt32(std::uint32_t l);
std::uint32_t LittleUInt32(std::uint32_t l);
std::uint64_t BigUInt64(std::uint64_t l);
std::uint64_t LittleUInt64(std::uint64_t l);

float	BigFloat(float l);
float	LittleFloat(float l);
double	BigDouble(double l);
double	LittleDouble(double l);

void	BigRevBytes(void *bp, int elsize, int elcount);
void	LittleRevBytes(void *bp, int elsize, int elcount);
void	LittleBitField(void *bp, int elsize);


template <typename E>
static std::underlying_type_t<E> EnumValue(E e)
{
    //static_assert(std::is_enum<E>::value, "E must be enum or enum class !");
    return static_cast<std::underlying_type_t<E>>(e);
}

int Snprintf(char* buf, std::size_t buflen, const char* format, ...)
#ifdef __GNUC__
__attribute__((format(printf, 3, 4)))
#endif
;

int Vsnprintf(char* buf, std::size_t buflen, const char* format, va_list ap)
#ifdef __GNUC__
__attribute__((format(printf, 3, 0)))
#endif
;

void Gettimeofday(struct timeval *tp);

//////////////////////////////////////////////////////////////////////////
}


namespace zylib {
namespace detail {
void	Swap_Init();

} // namespace detail
} // namespace zylib
