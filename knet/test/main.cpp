
#include <iostream>
#include <memory>

#include <boost/asio.hpp>

#include "knet/knet.h"

int main()
{
    auto p = std::make_shared<knet::UniformNetwork>();
    p->Init();
    return 0;
}
