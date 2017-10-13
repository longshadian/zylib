#include "Kafkacpp.h"

#include <memory>
#include <thread>
#include <iostream>

int main()
{
    auto conf = std::make_shared<kafkacpp::Conf>();
    conf->set("bootstrap.servers", "127.0.0.1:9092");
    if (!conf->set("acks", "all")) {
        std::cout << "set acks fail\n";
        return 0;
    }

    std::string err_str{};
    auto producer = conf->createProduer(&err_str);
    auto topic = producer->createTopic("tp.test");

    std::thread t{ [producer] 
        {
            std::cout << "thread_id: " << std::this_thread::get_id() << "\n";
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds{ 10 });
                producer->poll(0);
            }

        } };

    while (true) {
        auto str = std::to_string(std::time(nullptr));
        auto tnow = std::chrono::system_clock::now();
        producer->produce(*topic, str.c_str(), str.length(), 
            [str, tnow] (rd_kafka_t* kafka, const rd_kafka_message_t* kafka_msg)
            {
                auto tnow_ex = std::chrono::system_clock::now();
                auto n = std::chrono::duration_cast<std::chrono::milliseconds>(tnow_ex - tnow).count();
                std::cout << "thread_id:" << std::this_thread::get_id()
                    << "   str: " << str 
                    << " cost:" << n 
                    << "   err: " << kafka_msg->err << "\n";
            });
        std::this_thread::sleep_for(std::chrono::seconds{ 2 });
    }

    return 0;
}
