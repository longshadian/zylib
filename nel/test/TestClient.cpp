#include <cstdlib>
#include <cstring>
#include <iostream>
#include <chrono>
#include <boost/asio.hpp>

#include "Types.h"
#include "Nlnet.h"

using boost::asio::ip::tcp;

enum { max_length = 1024 };

std::vector<uint8_t> parseMsg(const nlnet::CMessage& msg)
{
    std::vector<uint8_t> all{};
    auto body = msg.serializeToArray();
    all.resize(4 + body.size());

    int32_t len = (int32_t)body.size() + 4;
    std::memcpy(all.data(), &len, 4);
    std::memcpy(all.data() + 4, body.data(), body.size());
    return all;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
  try
  {
    boost::asio::io_service io_service;

    tcp::socket s(io_service);
    tcp::resolver resolver(io_service);
    boost::asio::connect(s, resolver.resolve({"127.0.0.1", "22001"}));

    while (true) {
        std::cout << "message:  ";
        std::array<char, max_length> request{};
        request.fill(0);
        std::cin.getline(request.data(), max_length);

        int32_t request_length = (int32_t)std::strlen(request.data());
        if (request_length > 0) {
            std::string req_content = request.data();
            auto send_tm = std::chrono::system_clock::now();

            nlnet::CMessage req_msg{"_REQ_TEST", req_content};
            auto req_binary = parseMsg(req_msg);

            std::cout << "req len:" << req_binary.size()
                << " name:" << req_msg.getMsgName()
                << " data:" << req_msg.getData()
                << "\n";
            boost::asio::write(s, boost::asio::buffer(req_binary));

            std::array<char, 4> rsp_head{};
            rsp_head.fill(0);
            boost::asio::read(s, boost::asio::buffer(rsp_head));
            int32_t rsp_len = 0;
            std::memcpy(&rsp_len, rsp_head.data(), rsp_head.size());

            if (4 < rsp_len) {
                std::vector<uint8_t> rsp_body;
                rsp_body.resize(rsp_len - 4);
                boost::asio::read(s,
                    boost::asio::buffer(rsp_body));

                nlnet::CMessage rsp_msg{};
                rsp_msg.parseFromArray(rsp_body);

                std::cout << "received:" << rsp_msg.m_msg_name 
                    << "        data:" << rsp_msg.m_data << "\n";

                std::cout << "cost:" << 
                    std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now() - send_tm).count();
                std::cout << "\n\n";
            }
        }
    }
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
