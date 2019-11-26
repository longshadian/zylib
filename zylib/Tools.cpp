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

#if defined(_WIN32)
#include <windows.h>
#else
#endif

#include "zylib/Sys.h"

namespace zylib {

void Init()
{
    detail::Swap_Init();
}

std::vector<std::string> StrSplit(const std::string& s, char c)
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

std::size_t StrReplace(std::string* str, char src, char dest)
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

std::size_t StrRemove(std::string* str, char src)
{
    auto it = std::remove(str->begin(), str->end(), src);
    auto n = std::distance(it, str->end());
    str->erase(it, str->end());
    return n;
}

// can't just use function pointers, or dll linkage can mess up
static std::int16_t(*BigInt16_Ptr)(std::int16_t l);
static std::int16_t(*LittleInt16_Ptr)(std::int16_t l);
static std::int32_t(*BigInt32_Ptr)(std::int32_t l);
static std::int32_t(*LittleInt32_Ptr)(std::int32_t l);
static std::int64_t(*BigInt64_Ptr)(std::int64_t l);
static std::int64_t(*LittleInt64_Ptr)(std::int64_t l);

static std::uint16_t(*BigUInt16_Ptr)(std::uint16_t l);
static std::uint16_t(*LittleUInt16_Ptr)(std::uint16_t l);
static std::uint32_t(*BigUInt32_Ptr)(std::uint32_t l);
static std::uint32_t(*LittleUInt32_Ptr)(std::uint32_t l);
static std::uint64_t(*BigUInt64_Ptr)(std::uint64_t l);
static std::uint64_t(*LittleUInt64_Ptr)(std::uint64_t l);

static float(*BigFloat_Ptr)(float l);
static float(*LittleFloat_Ptr)(float l);
static double(*BigDouble_Ptr)(double l);
static double(*LittleDouble_Ptr)(double l);

static void(*BigRevBytes_Ptr)(void *bp, int elsize, int elcount);
static void(*LittleRevBytes_Ptr)(void *bp, int elsize, int elcount);
static void(*LittleBitField_Ptr)(void *bp, int elsize);
static void(*SixtetsForInt_Ptr)(std::uint8_t *out, std::int32_t src);
static int(*IntForSixtets_Ptr)(std::uint8_t *in);

std::int16_t BigInt16(std::int16_t l) { return BigInt16_Ptr(l); }
std::int16_t LittleInt16(std::int16_t l) { return LittleInt16_Ptr(l); }
std::int32_t BigInt32(std::int32_t l) { return BigInt32_Ptr(l); }
std::int32_t LittleInt32(std::int32_t l) { return LittleInt32_Ptr(l); }
std::int64_t BigInt64(std::int64_t l) { return BigInt64_Ptr(l);  }
std::int64_t LittleInt64(std::int64_t l) { return LittleInt64_Ptr(l); }

std::uint16_t BigUInt16(std::uint16_t l) { return BigUInt16_Ptr(l); }
std::uint16_t LittleUInt16(std::uint16_t l) { return LittleUInt16_Ptr(l); }
std::uint32_t BigUInt32(std::uint32_t l) { return BigUInt32_Ptr(l); }
std::uint32_t LittleUInt32(std::uint32_t l) { return LittleUInt32_Ptr(l); }
std::uint64_t BigUInt64(std::uint64_t l) { return BigUInt64_Ptr(l); }
std::uint64_t LittleUInt64(std::uint64_t l) { return LittleUInt64_Ptr(l); }


float	BigFloat(float l) { return BigFloat_Ptr(l); }
float	LittleFloat(float l) { return LittleFloat_Ptr(l); }
double	BigDouble(double l) { return BigDouble_Ptr(l); }
double	LittleDouble(double l) { return LittleDouble_Ptr(l); }

void	BigRevBytes(void *bp, int elsize, int elcount) { BigRevBytes_Ptr(bp, elsize, elcount); }
void	LittleRevBytes(void *bp, int elsize, int elcount) { LittleRevBytes_Ptr(bp, elsize, elcount); }
void	LittleBitField(void *bp, int elsize) { LittleBitField_Ptr(bp, elsize); }

void	SixtetsForInt(std::uint8_t *out, int src) { SixtetsForInt_Ptr(out, src); }
int		IntForSixtets(std::uint8_t *in) { return IntForSixtets_Ptr(in); }

static std::int16_t Int16Swap(std::int16_t l) 
{
    std::uint8_t b1, b2;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    return (std::int16_t)(b1 << 8) + b2;
}

static std::int16_t Int16NoSwap(std::int16_t l) 
{
    return l;
}

static std::int32_t Int32Swap(std::int32_t l) 
{
    std::uint8_t    b1, b2, b3, b4;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    b3 = (l >> 16) & 255;
    b4 = (l >> 24) & 255;
    return ((std::int32_t)b1 << 24) + ((std::int32_t)b2 << 16) + ((std::int32_t)b3 << 8) + b4;
}

static std::int32_t	Int32NoSwap(std::int32_t l) 
{
    return l;
}

static std::int64_t Int64Swap(std::int64_t l) 
{
    std::uint8_t    b1, b2, b3, b4, b5, b6, b7, b8;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    b3 = (l >> 16) & 255;
    b4 = (l >> 24) & 255;
    b5 = (l >> 32) & 255;
    b6 = (l >> 40) & 255;
    b7 = (l >> 48) & 255;
    b8 = (l >> 56) & 255;
    return ((std::int64_t)b1 << 56) + ((std::int64_t)b2 << 48) + ((std::int64_t)b3 << 40) + ((std::int64_t)b4 << 32) +
        ((std::int64_t)b5 << 24) + ((std::int64_t)b6 << 16) + ((std::int64_t)b7 << 8) + b8;
}

static std::int64_t	Int64NoSwap(std::int64_t l) 
{
    return l;
}

static std::uint16_t UInt16Swap(std::uint16_t l) 
{
    std::uint8_t b1, b2;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    return (std::uint16_t)(b1 << 8) + b2;
}

static std::uint16_t UInt16NoSwap(std::uint16_t l) 
{
    return l;
}

static std::uint32_t UInt32Swap(std::uint32_t l) 
{
    std::uint8_t    b1, b2, b3, b4;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    b3 = (l >> 16) & 255;
    b4 = (l >> 24) & 255;
    return ((std::uint32_t)b1 << 24) + ((std::uint32_t)b2 << 16) + ((std::uint32_t)b3 << 8) + b4;
}

static std::uint32_t UInt32NoSwap(std::uint32_t l) 
{
    return l;
}

static std::uint64_t UInt64Swap(std::uint64_t l) 
{
    std::uint8_t    b1, b2, b3, b4, b5, b6, b7, b8;
    b1 = l & 255;
    b2 = (l >> 8) & 255;
    b3 = (l >> 16) & 255;
    b4 = (l >> 24) & 255;
    b5 = (l >> 32) & 255;
    b6 = (l >> 40) & 255;
    b7 = (l >> 48) & 255;
    b8 = (l >> 56) & 255;
    return ((std::uint64_t)b1 << 56) + ((std::uint64_t)b2 << 48) + ((std::uint64_t)b3 << 40) + ((std::uint64_t)b4 << 32) +
        ((std::uint64_t)b5 << 24) + ((std::uint64_t)b6 << 16) + ((std::uint64_t)b7 << 8) + b8;
}

static std::uint64_t UInt64NoSwap(std::uint64_t l) 
{
    return l;
}

static float FloatSwap(float f) 
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

static float FloatNoSwap(float f) 
{
    return f;
}

static double DoubleSwap(double f) 
{
    union {
        double	        f;
        std::uint8_t	b[8];
    } dat1, dat2;

    dat1.f = f;
    dat2.b[0] = dat1.b[7];
    dat2.b[1] = dat1.b[6];
    dat2.b[2] = dat1.b[5];
    dat2.b[3] = dat1.b[4];
    dat2.b[4] = dat1.b[3];
    dat2.b[5] = dat1.b[2];
    dat2.b[6] = dat1.b[1];
    dat2.b[7] = dat1.b[0];
    return dat2.f;
}

static double DoubleNoSwap(double f) 
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

static void RevBytesNoSwap(void *bp, int elsize, int elcount) 
{
    return;
}

static void RevBitFieldNoSwap(void *bp, int elsize) 
{
    return;
}

static void SixtetsForIntLittle(std::uint8_t* out, std::int32_t src) 
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
void SixtetsForIntBig(std::uint8_t* out, std::int32_t src) {
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
    std::FILE* fp = Fopen(f, "rb");
    if (!fp)
        return "";

    std::string content;
    std::array<char, 1024 * 16> buffer{};
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
    std::FILE* f = Fopen(path.c_str(), "rb");
    if (!f) {
        return false;
    }

    bool succ = true;
    std::string content;
    std::array<char, 1024 * 16> buffer;
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
 
std::string ToHex(const void* data, std::size_t len)
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
#if defined(_WIN32)
    localtime_s(output, t);
#else
    localtime_r(t, output);
#endif
    return output;
}

std::string Localtime_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    Localtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d"
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

std::string Localtime_YYYYMMDD_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    Localtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d"
        , cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

std::string Localtime_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    Gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    Localtime(&cur_t, &cur_tm);

    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%06d",
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
}

std::string Localtime_YYYYMMDD_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    Gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    Localtime(&cur_t, &cur_tm);
    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d.%06d",
        cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday,
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
}

std::string UTC_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    Gmtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d"
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

std::string UTC_YYYYMMDD_HHMMSS(const std::time_t* t)
{
    struct tm cur_tm = { 0 };
    Gmtime(t, &cur_tm);
    char buffer[64] = { 0 };

    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d"
        , cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday
        , cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec
    );
    return std::string(buffer);
}

std::string UTC_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    Gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    Gmtime(&cur_t, &cur_tm);

    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d.%06d",
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
}

std::string UTC_YYYYMMDD_HHMMSS_F()
{
    struct timeval cur_tv = { 0 };
    Gettimeofday(&cur_tv);

    struct tm cur_tm = { 0 };
    std::time_t cur_t = static_cast<std::time_t>(cur_tv.tv_sec);
    Gmtime(&cur_t, &cur_tm);
    char buffer[64] = { 0 };
    snprintf(buffer, sizeof(buffer), "%04d/%02d/%02d %02d:%02d:%02d.%06d",
        cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday,
        cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec,
        static_cast<std::int32_t>(cur_tv.tv_usec)
    );
    return std::string(buffer);
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
    r = _vsnprintf_s(buf, buflen, buflen, format, ap);
    if (r < 0)
        r = _vscprintf(format, ap);
#else
    r = vsnprintf(buf, buflen, format, ap);
#endif
    buf[buflen - 1] = '\0';
    return r;
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

void Gmtime(const std::time_t* t, struct tm* output)
{
#if defined(_WIN32)
    gmtime_s(output, t);
#else
    gmtime_r(t, output);
#endif
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
        BigInt16_Ptr = Int16Swap;
        LittleInt16_Ptr = Int16NoSwap;
        BigInt32_Ptr = Int32Swap;
        LittleInt32_Ptr = Int32NoSwap;
        BigInt64_Ptr = Int64Swap;
        LittleInt64_Ptr = Int64NoSwap;

        BigUInt16_Ptr = UInt16Swap;
        LittleUInt16_Ptr = UInt16NoSwap;
        BigUInt32_Ptr = UInt32Swap;
        LittleUInt32_Ptr = UInt32NoSwap;
        BigUInt64_Ptr = UInt64Swap;
        LittleUInt64_Ptr = UInt64NoSwap;

        BigFloat_Ptr = FloatSwap;
        LittleFloat_Ptr = FloatNoSwap;
        BigDouble_Ptr = DoubleSwap;
        LittleDouble_Ptr = DoubleNoSwap;

        BigRevBytes_Ptr = RevBytesSwap;
        LittleRevBytes_Ptr = RevBytesNoSwap;
        LittleBitField_Ptr = RevBitFieldNoSwap;
        SixtetsForInt_Ptr = SixtetsForIntLittle;
        IntForSixtets_Ptr = IntForSixtetsLittle;
    } else {
        // big endian ex: ppc
        BigInt16_Ptr = Int16NoSwap;
        LittleInt16_Ptr = Int16Swap;
        BigInt32_Ptr = Int32NoSwap;
        LittleInt32_Ptr = Int32Swap;
        BigInt64_Ptr = Int64NoSwap;
        LittleInt64_Ptr = Int64Swap;

        BigUInt16_Ptr = UInt16NoSwap;
        LittleUInt16_Ptr = UInt16Swap;
        BigUInt32_Ptr = UInt32NoSwap;
        LittleUInt32_Ptr = UInt32Swap;
        BigUInt64_Ptr = UInt64NoSwap;
        LittleUInt64_Ptr = UInt64Swap;

        BigFloat_Ptr = FloatNoSwap;
        LittleFloat_Ptr = FloatSwap;
        BigDouble_Ptr = DoubleNoSwap;
        LittleDouble_Ptr = DoubleSwap;

        BigRevBytes_Ptr = RevBytesNoSwap;
        LittleRevBytes_Ptr = RevBytesSwap;
        LittleBitField_Ptr = RevBitFieldSwap;
        SixtetsForInt_Ptr = SixtetsForIntBig;
        IntForSixtets_Ptr = IntForSixtetsBig;
    }
}

} // namespace detail
} // namespace zylib

