#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>

#include "Nlnet.h"

using boost::asio::ip::tcp;

struct Temp
{
    bool        m_bool;
    int8_t      m_int8;
    int16_t     m_int16;
    int32_t     m_int32;
    int64_t     m_int64;

    uint8_t     m_uint8;
    uint16_t    m_uint16;
    uint32_t    m_uint32;
    uint64_t    m_uint64;

    float       m_float;
    double      m_double;
    long double m_long_double;

    bool operator==(const Temp& rhs) const
    {
        return m_bool == rhs.m_bool
            && m_int8 == rhs.m_int8
            && m_int16 == rhs.m_int16
            && m_int32 == rhs.m_int32
            && m_int64 == rhs.m_int64
            && m_uint8 == rhs.m_uint8
            && m_uint16 == rhs.m_uint16
            && m_uint32 == rhs.m_uint32
            && m_uint64 == rhs.m_uint64
            && m_float == rhs.m_float
            && m_double == rhs.m_double
            && m_long_double == rhs.m_long_double;
    }
};

void fun()
{
    Temp x1{};
    x1.m_bool = true;
    x1.m_int8 = -99;
    x1.m_int16 = -9999;
    x1.m_int32 = -109993123;
    x1.m_int64 = -1233333333333333u;

    x1.m_uint8 = 112;
    x1.m_uint16 = 12312;
    x1.m_uint32 = 1111111111u;
    x1.m_uint64 = 11111111111111111111u;

    x1.m_float = 3.1415f;
    x1.m_double = 1111113.133123;
    x1.m_long_double = 1111111.333333;

    nlnet::ByteBuf buf{};
    buf << x1.m_bool
        << x1.m_int8
        << x1.m_int16
        << x1.m_int32
        << x1.m_int64
        << x1.m_uint8
        << x1.m_uint16
        << x1.m_uint32
        << x1.m_uint64
        << x1.m_float
        << x1.m_double
        << x1.m_long_double;

    Temp x2{};
    std::cout << "total:    " << buf.byteSize() << "\n";

    buf >> x2.m_bool;
    buf.shrinkToFit();
    std::cout << "m_bool:   " << buf.byteSize() << " " <<x2.m_bool << "\n";

    buf >> x2.m_int8;
    buf.shrinkToFit();
    std::cout << "m_int8:   " << buf.byteSize() << " " << (int)x2.m_int8 << "\n";

    buf >> x2.m_int16;
    buf.shrinkToFit();
    std::cout << "m_int16:  " << buf.byteSize() << " " << x2.m_int16 << "\n";

    buf >> x2.m_int32;
    buf.shrinkToFit();
    std::cout << "m_int32:  " << buf.byteSize() << " " << x2.m_int32 << "\n";

    buf >> x2.m_int64;
    buf.shrinkToFit();
    std::cout << "m_int64:  " << buf.byteSize() << " " << x2.m_int64 << "\n";

    buf >> x2.m_uint8;
    buf.shrinkToFit();
    std::cout << "m_int8:   " << buf.byteSize() << " " << (int)x2.m_uint8 << "\n";

    buf >> x2.m_uint16;
    buf.shrinkToFit();
    std::cout << "m_int16:  " << buf.byteSize() << " " << x2.m_uint16 << "\n";

    buf >> x2.m_uint32;
    buf.shrinkToFit();
    std::cout << "m_int32:  " << buf.byteSize() << " " << x2.m_uint32 << "\n";

    buf >> x2.m_uint64;
    buf.shrinkToFit();
    std::cout << "m_int64:  " << buf.byteSize() << " " << x2.m_uint64 << "\n";

    buf >> x2.m_float;
    buf.shrinkToFit();
    std::cout << "m_float:  " << buf.byteSize() << " " << x2.m_float << "\n";

    buf >> x2.m_double;
    buf.shrinkToFit();
    std::cout << "m_double:  " << buf.byteSize() << " " << x2.m_double << "\n";

    buf >> x2.m_long_double;
    buf.shrinkToFit();
    std::cout << "m_long_double:  " << buf.byteSize() << " " << x2.m_long_double << "\n";
    
    buf.shrinkToFit();
    std::cout << "x1 == x2:" << (x1 == x2) << "\n";
}

void fun2()
{
    int32_t v = 123512;
    std::string str = "fhhhonsdonfa;sdng";
    nlnet::ByteBuf buf{};
    buf << v;

    int32_t v_ex = 0;
    buf.read(&v_ex);
    std::cout << v_ex << "\t" << buf.byteSize() << "\n";

    v_ex = 0;
    buf.read(&v_ex);
    std::cout << v_ex << "\t" << buf.byteSize() << "\n";
    v_ex = 0;

    buf >> v_ex;
    std::cout << v_ex << "\t" << buf.byteSize() << "\n";

    buf << str;

    std::string str_ex{};
    buf >> str_ex;
    std::cout << str_ex << "\t" << (str == str_ex) << " " << buf.byteSize() << "\n";

    buf << str_ex;
    std::vector<uint8_t> data{};
    buf >> data;
    for (auto c : data) {
        std::cout << (char)c;
    }
    std::cout << "\n";
    std::cout << buf.byteSize() << "\n";
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    try {
        fun();
        std::cout << "\ntest fun2:\n";
        fun2();

        /*
        std::cout << std::boolalpha;
        std::cout << "temp      " << std::is_fundamental<Temp>::value << '\n';
        std::cout << "bool      " << std::is_fundamental<bool>::value << '\n';
        std::cout << "int       " << std::is_fundamental<int>::value << '\n';
        std::cout << "int&      " << std::is_fundamental<int&>::value << '\n';
        std::cout << "int*      " << std::is_fundamental<int*>::value << '\n';
        std::cout << "float     " << std::is_fundamental<float>::value << '\n';
        std::cout << "falot&    " << std::is_fundamental<float&>::value << '\n';
        std::cout << "flaot*    " << std::is_fundamental<float*>::value << '\n';
        std::cout << "nullptr   " << std::is_fundamental<void>::value << '\n';
        */

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
