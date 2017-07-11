#include <iostream>
#include <string.h>
#include <sstream>
#include <stdlib.h>

std::string decToOct(int64_t t)
{

}

int64_t octToDec(std::string s)
{

}

int main()
{
	//std::cout << converIP("192.168.231.2") << std::endl;
    {
        uint64_t id = 12195900;
        //id ^= 0x1A2B3C;
        std::ostringstream ostm{};
        ostm << std::oct << id;
        auto s = ostm.str();
        std::cout << s << "\n";
        auto v = atoi(s.c_str());
        std::cout << (v^0x1A2B3C) << "\n";
    }
	return 0;
}



