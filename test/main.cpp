
#include <cstring>

#include <iostream>
#include <sstream>
#include <vector>

#include <boost/asio.hpp>

int main()
{

    try {
        int32_t a = 10;
        int32_t b = 100;
        boost::asio::streambuf buff;
        buff << a;
        buff << b;

        int32_t a_ex = 0;
        int32_t b_ex = 0;
        std::cout << buff.size() << "\n";
    } catch (std::exception e) {
        std::cout << "exception " << e.what();
    }
	return 0;
}



