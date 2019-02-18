#include <iostream>

#include <cstdlib>
#include <limits>


template <typename T>
struct Convert;

template <>
struct Convert<long long>
{
    static long long cvt(const char* p)
    {
        return std::strtoll(p, nullptr, 10);
    }
};

template <>
struct Convert<unsigned long long>
{
    static unsigned long long cvt(const char* p)
    {
        return std::strtoull(p, nullptr, 10);
    }
};

template <>
struct Convert<long>
{
    static long cvt(const char* p)
    {
        return std::strtol(p, nullptr, 10);
    }
};

template <>
struct Convert<unsigned long>
{
    static unsigned long cvt(const char* p)
    {
        return std::strtoul(p, nullptr, 10);
    }
};

template <>
struct Convert<int>
{
    static int cvt(const char* p)
    {
        return static_cast<int>(Convert<long>::cvt(p));
    }
};

template <>
struct Convert<unsigned int>
{
    static unsigned int cvt(const char* p)
    {
        return static_cast<unsigned int>(Convert<unsigned long>::cvt(p));
    }
};

template <>
struct Convert<short>
{
    static short cvt(const char* p)
    {
        return static_cast<short>(Convert<long>::cvt(p));
    }
};

template <>
struct Convert<unsigned short>
{
    static unsigned short cvt(const char* p)
    {
        return static_cast<unsigned short>(Convert<unsigned long>::cvt(p));
    }
};

template <>
struct Convert<char>
{
    static char cvt(const char* p)
    {
        return static_cast<char>(Convert<long>::cvt(p));
    }
};

template <>
struct Convert<unsigned char>
{
    static unsigned char cvt(const char* p)
    {
        return static_cast<unsigned char>(Convert<unsigned long>::cvt(p));
    }
};


template <>
struct Convert<float>
{
    static float cvt(const char* p)
    {
        return std::strtof(p, nullptr);
    }
};

template <>
struct Convert<double>
{
    static double cvt(const char* p)
    {
        return std::strtod(p, nullptr);
    }
};

template <>
struct Convert<long double>
{
    static long double cvt(const char* p)
    {
        return std::strtold(p, nullptr);
    }
};


template <typename T>
T strToArithmetic(const char* p)
{
    static_assert(std::is_arithmetic<T>::value, "T must be integral type or floating point type!");
    if (!p)
        return T();
    return Convert<T>::cvt(p);
}

/*
int main()
{
    std::cout << "char max:               " << (int)std::numeric_limits<char>::max() << "\n";
    std::cout << "unsigned char max       " << (int)std::numeric_limits<unsigned char>::max() << "\n";
    std::cout << "short max:              " << std::numeric_limits<short>::max() << "\n";
    std::cout << "unsigned short max      " << std::numeric_limits<unsigned short>::max() << "\n";
    std::cout << "int max:                " << std::numeric_limits<int>::max() << "\n";
    std::cout << "unsigned int max:       " << std::numeric_limits<unsigned int>::max() << "\n";
    std::cout << "long max:               " << std::numeric_limits<long>::max() << "\n";
    std::cout << "long long max:          " << std::numeric_limits<long long>::max() << "\n";
    std::cout << "unsigned long max:      " << std::numeric_limits<unsigned long>::max() << "\n";
    std::cout << "unsigned long long max: " << std::numeric_limits<unsigned long long>::max() << "\n";

    std::cout << std::atol("2147483648") << "\n";
    std::cout << static_cast<int>(std::atol("2147483648")) << "\n";
    std::cout << strToArithmetic<int>("2147483648") << "\n";
    std::cout << strToArithmetic<unsigned int>("2147483648") << "\n";
    std::cout << strToArithmetic<long>("2147483648") << "\n";

    std::cout << "=============\n";
    std::cout << strToArithmetic<int32_t>("12147483648") << "\n";
    std::cout << strToArithmetic<int64_t>("2147483648") << "\n";

    std::cout << std::to_string(strToArithmetic<float>("12147483648.1234567")) << "\n";
    std::cout << std::to_string(strToArithmetic<double>("12147483648.1234567")) << "\n";
    std::cout << std::to_string(strToArithmetic<long double>("12147483648.1234567")) << "\n";

    std::cout << strToArithmetic<int>("") << "\n";
    std::cout << strToArithmetic<float>("") << "\n";

    return 0;
}
*/
