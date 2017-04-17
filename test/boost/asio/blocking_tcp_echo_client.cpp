//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service;

    tcp::socket s(io_service);
    tcp::resolver resolver(io_service);
    boost::asio::connect(s, resolver.resolve({"127.0.0.1", "21010"}));

    while (true) {
        std::cout << "message:  ";
        std::array<char, max_length> request{};
        request.fill(0);
        std::cin.getline(request.data(), max_length);

        int32_t request_length = (int32_t)std::strlen(request.data());
        if (request_length > 0) {
            int32_t total_len = request_length + 4;
            boost::asio::write(s, boost::asio::buffer(&total_len, 4));
            boost::asio::write(s, boost::asio::buffer(request.data(), request_length));

            char reply_head[4] = {0};
            boost::asio::read(s, boost::asio::buffer(reply_head, 4));
            int32_t reply_total = 0;
            std::memcpy(&reply_total, reply_head, 4);
            if (4 < reply_total && reply_total < 100) {

                std::vector<char> reply_body;
                reply_body.resize(reply_total - 4);
                size_t reply_length = boost::asio::read(s,
                    boost::asio::buffer(reply_body));
                std::cout << "received: ";
                std::cout.write(reply_body.data(), reply_body.size());
                std::cout << "\n\n";
            }
        }
    }
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
