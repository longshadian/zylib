#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <functional>

#include "X.h"

void fun1() { }
void fun2() { }

static std::unordered_map<std::string, std::function<void()>> call_array =
{
    {xx::a, &fun1},
    {xx::b, &fun2},
};

int main(int argc, char* argv[])
{
    std::cout << call_array.size() << "\n";
    return 0;
}
