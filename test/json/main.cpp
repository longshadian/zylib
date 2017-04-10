#include <iostream>
#include <string>

#include <json/json.h>

using namespace Json;

int main()
{
	Json::Value v;
	v["a"] = 1;
	v["b"] = "bb";
	Json::FastWriter writer{};
	auto s = writer.write(v);
	if (s.back() == '\n')
		s.pop_back();
	std::cout << s;
    return 0;
}
