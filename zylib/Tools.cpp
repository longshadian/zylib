#include "Tools.h"

#include <cstdlib>
#include <ctime>
#include <cstdarg>
#include <cstring>
#include <sstream>
#include <memory>
#include <array>
#include <algorithm>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

namespace zylib {

void Init()
{
    detail::Swap_Init();
}

std::vector<std::string> StringSplit(const std::string& s, char c)
{
    std::vector<std::string> out;
    if (s.empty())
        return out;

    std::istringstream istm(s);
    std::string temp;
    while (std::getline(istm, temp, c)) {
        out.push_back(temp);
    }
    return out;
}

size_t StringReplace(std::string* str, char src, char dest)
{
    size_t t = 0;
    std::transform(str->begin(), str->end(), str->begin(),
        [&t, src, dest](char& c)
        {
            if (c == src) {
                c = dest;
                ++t;
            }
            return c;
        } );
    return t;
}

void StringRemove(std::string* str, char src)
{
    str->erase(std::remove(str->begin(), str->end(), src), str->end());
}

// can't just use function pointers, or dll linkage can mess up
static short(*_BigShort)(short l);
static short(*_LittleShort)(short l);
static int(*_BigLong)(int l);
static int(*_LittleLong)(int l);
static float(*_BigFloat)(float l);
static float(*_LittleFloat)(float l);
static void(*_BigRevBytes)(void *bp, int elsize, int elcount);
static void(*_LittleRevBytes)(void *bp, int elsize, int elcount);
static void(*_LittleBitField)(void *bp, int elsize);
static void(*_SixtetsForInt)(std::uint8_t *out, int src);
static int(*_IntForSixtets)(std::uint8_t *in);

short	BigShort(short l) { return _BigShort(l); }
short	LittleShort(short l) { return _LittleShort(l); }
int		BigLong(int l) { return _BigLong(l); }
int		LittleLong(int l) { return _LittleLong(l); }
float	BigFloat(float l) { return _BigFloat(l); }
float	LittleFloat(float l) { return _LittleFloat(l); }
void	BigRevBytes(void *bp, int elsize, int elcount) { _BigRevBytes(bp, elsize, elcount); }
void	LittleRevBytes(void *bp, int elsize, int elcount) { _LittleRevBytes(bp, elsize, elcount); }
void	LittleBitField(void *bp, int elsize) { _LittleBitField(bp, elsize); }

void	SixtetsForInt(std::uint8_t *out, int src) { _SixtetsForInt(out, src); }
int		IntForSixtets(std::uint8_t *in) { return _IntForSixtets(in); }

short ShortSwap(short l) 
{
    std::uint8_t    b1, b2;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    return (b1 << 8) + b2;
}

short ShortNoSwap(short l) 
{
    return l;
}

int LongSwap(int l) 
{
    std::uint8_t    b1, b2, b3, b4;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    b3 = (l >> 16) & 255;
    b4 = (l >> 24) & 255;
    return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

int	LongNoSwap(int l) 
{
    return l;
}

float FloatSwap(float f) 
{
    union {
        float	        f;
        std::uint8_t	b[4];
    } dat1, dat2;

    dat1.f = f;
    dat2.b[0] = dat1.b[3];
    dat2.b[1] = dat1.b[2];
    dat2.b[2] = dat1.b[1];
    dat2.b[3] = dat1.b[0];
    return dat2.f;
}

float FloatNoSwap(float f) 
{
    return f;
}

/*
=====================================================================
RevBytesSwap

Reverses byte order in place.

INPUTS
   bp       bytes to reverse
   elsize   size of the underlying data type
   elcount  number of elements to swap

RESULTS
   Reverses the byte order in each of elcount elements.
===================================================================== */
void RevBytesSwap(void *bp, int elsize, int elcount) 
{
    unsigned char *p, *q;

    p = (unsigned char *)bp;

    if (elsize == 2) {
        q = p + 1;
        while (elcount--) {
            *p ^= *q;
            *q ^= *p;
            *p ^= *q;
            p += 2;
            q += 2;
        }
        return;
    }

    while (elcount--) {
        q = p + elsize - 1;
        while (p < q) {
            *p ^= *q;
            *q ^= *p;
            *p ^= *q;
            ++p;
            --q;
        }
        p += elsize >> 1;
    }
}

/*
 =====================================================================
 RevBytesSwap

 Reverses byte order in place, then reverses bits in those bytes

 INPUTS
 bp       bitfield structure to reverse
 elsize   size of the underlying data type

 RESULTS
 Reverses the bitfield of size elsize.
 ===================================================================== */
void RevBitFieldSwap(void *bp, int elsize) 
{
    int i;
    unsigned char *p, t, v;

    LittleRevBytes(bp, elsize, 1);

    p = (unsigned char *)bp;
    while (elsize--) {
        v = *p;
        t = 0;
        for (i = 7; i; i--) {
            t <<= 1;
            v >>= 1;
            t |= v & 1;
        }
        *p++ = t;
    }
}

void RevBytesNoSwap(void *bp, int elsize, int elcount) 
{
    return;
}

void RevBitFieldNoSwap(void *bp, int elsize) 
{
    return;
}

void SixtetsForIntLittle(std::uint8_t *out, int src) 
{
    std::uint8_t *b = (std::uint8_t *)&src;
    out[0] = (b[0] & 0xfc) >> 2;
    out[1] = ((b[0] & 0x3) << 4) + ((b[1] & 0xf0) >> 4);
    out[2] = ((b[1] & 0xf) << 2) + ((b[2] & 0xc0) >> 6);
    out[3] = b[2] & 0x3f;
}

/*
================
SixtetsForIntBig
TTimo: untested - that's the version from initial base64 encode
================
*/
void SixtetsForIntBig(std::uint8_t *out, int src) {
    for (int i = 0; i < 4; i++) {
        out[i] = src & 0x3f;
        src >>= 6;
    }
}

int IntForSixtetsLittle(std::uint8_t *in) {
    int ret = 0;
    std::uint8_t *b = (std::uint8_t *)&ret;
    b[0] |= in[0] << 2;
    b[0] |= (in[1] & 0x30) >> 4;
    b[1] |= (in[1] & 0xf) << 4;
    b[1] |= (in[2] & 0x3c) >> 2;
    b[2] |= (in[2] & 0x3) << 6;
    b[2] |= in[3];
    return ret;
}

/*
================
IntForSixtetsBig
TTimo: untested - that's the version from initial base64 decode
================
*/
int IntForSixtetsBig(std::uint8_t *in) {
    int ret = 0;
    ret |= in[0];
    ret |= in[1] << 6;
    ret |= in[2] << 2 * 6;
    ret |= in[3] << 3 * 6;
    return ret;
}

std::string CatFile(const char* f)
{
    std::FILE* fp = std::fopen(f, "rb");
    if (!fp)
        return "";

    std::string content;
    std::array<char, 1024> buffer{};
    while (1) {
        std::size_t readn = std::fread(buffer.data(), 1, buffer.size(), fp);
        if (readn == 0)
            break;
        content.append(buffer.data(), buffer.data() + readn);
    }
    std::fclose(fp);
    return content;
}

bool CatFile(const std::string& path, std::string* out)
{
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) {
        return false;
    }

    bool succ = true;
    std::string content;
    std::array<char, 1024> buffer;
    std::uint64_t total = 0;
    while (true) {
        std::size_t readn = std::fread(buffer.data(), 1, buffer.size(), f);
        if (readn == 0) {
            break;
        }
        if (readn > buffer.size()) {
            succ = false;
            break;
        }
        total += readn;
        content.append(buffer.data(), buffer.data() + readn);
    }
    std::fclose(f);
    if (!succ)
        return succ;
    std::swap(*out, content);
    return true;
}

std::string ToUpperCase(const std::string& src)
{
    if (src.empty())
        return "";
    std::string dst;
    dst.resize(src.size());
    std::transform(src.begin(), src.end(), dst.begin(), [](int c) { return static_cast<char>(::toupper(c)); });
    return dst;
}

std::string ToLowerCase(const std::string& src)
{
    if (src.empty())
        return "";
    std::string dst;
    dst.resize(src.size());
    std::transform(src.begin(), src.end(), dst.begin(), [](int c) { return static_cast<char>(::tolower(c)); });
    return dst;
}
 
static std::string ToHex(const void* data, std::size_t len)
{
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (std::size_t i = 0; i != len; ++i) {
        unsigned int c = p[i];
        out << std::setw(2) << c;
    }
    return out.str();
}

struct tm* Localtime(const time_t* t, struct tm* output)
{
#ifdef WIN32
    localtime_s(output, t);
#else
    localtime_r(t, output);
#endif
    return output;
}

std::string LocaltimeYYYMMDD_HHMMSS(std::time_t t)
{
    struct tm cur_tm {};
    Localtime(&t, &cur_tm);
    char buffer[128] = { 0 };

    snprintf(buffer, sizeof(buffer), "%04d%02d%02d-%02d%02d%02d"
        , cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    std::string s = buffer;
    return s;
}

int Snprintf(char* buf, std::size_t buflen, const char* format, ...)
{
    int r;
    va_list ap;
    va_start(ap, format);
    r = Vsnprintf(buf, buflen, format, ap);
    va_end(ap);
    return r;
}

int Vsnprintf(char* buf, std::size_t buflen, const char* format, va_list ap)
{
    int r = 0;
    if (!buflen)
        return 0;
#if defined(_MSC_VER) || defined(_WIN32)
    r = _vsnprintf(buf, buflen, format, ap);
    if (r < 0)
        r = _vscprintf(format, ap);
#else
    r = vsnprintf(buf, buflen, format, ap);
#endif
    buf[buflen - 1] = '\0';
    return r;
}

void Localtime(std::time_t s, struct tm* tm)
{
#if defined(_WIN32)
    localtime_s(tm, &s);
#else
    (void)localtime_r(&s, tm);
#endif
}

void Gettimeofday(struct timeval* tp)
{
#if defined(_WIN32)
    uint64_t intervals;
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);

    /*
     * A file time is a 64-bit value that represents the number
     * of 100-nanosecond intervals that have elapsed since
     * January 1, 1601 12:00 A.M. UTC.
     *
     * Between January 1, 1970 (Epoch) and January 1, 1601 there were
     * 134744 days,
     * 11644473600 seconds or
     * 11644473600,000,000,0 100-nanosecond intervals.
     *
     * See also MSKB Q167296.
     */

    intervals = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    tp->tv_sec = (long)(intervals / 10000000);
    tp->tv_usec = (long)((intervals % 10000000) / 10);
#else
    ::gettimeofday(tp, nullptr);
#endif // defined(_WIN32)
}


} // namespace zylib


namespace zylib {
namespace detail {

void Swap_Init()
{
    std::uint8_t	swaptest[2] = { 1,0 };

    // set the byte swapping variables in a portable manner	
    if (*(short *)swaptest == 1) {
        // little endian ex: x86
        _BigShort = ShortSwap;
        _LittleShort = ShortNoSwap;
        _BigLong = LongSwap;
        _LittleLong = LongNoSwap;
        _BigFloat = FloatSwap;
        _LittleFloat = FloatNoSwap;
        _BigRevBytes = RevBytesSwap;
        _LittleRevBytes = RevBytesNoSwap;
        _LittleBitField = RevBitFieldNoSwap;
        _SixtetsForInt = SixtetsForIntLittle;
        _IntForSixtets = IntForSixtetsLittle;
    }
    else {
        // big endian ex: ppc
        _BigShort = ShortNoSwap;
        _LittleShort = ShortSwap;
        _BigLong = LongNoSwap;
        _LittleLong = LongSwap;
        _BigFloat = FloatNoSwap;
        _LittleFloat = FloatSwap;
        _BigRevBytes = RevBytesNoSwap;
        _LittleRevBytes = RevBytesSwap;
        _LittleBitField = RevBitFieldSwap;
        _SixtetsForInt = SixtetsForIntBig;
        _IntForSixtets = IntForSixtetsBig;
    }
}

} // namespace detail
} // namespace zylib

