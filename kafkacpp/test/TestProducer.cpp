#include "Kafkacpp.h"

#include <memory>
#include <thread>
#include <iostream>
#include <atomic>

int testProducer(kafkacpp::TopicPtr topic, kafkacpp::ProducerPtr producer)
{
    int32_t n_cb{ 0 };
    int32_t data_size = 1024;
    int32_t n = 100000;

    std::string str_data{};
    str_data.resize(data_size, 'a');
    auto tnow = std::chrono::system_clock::now();
    for (int32_t i = 0; i != n; ++i) {
        producer->produce(*topic, str_data.c_str(), str_data.length()
            , [tnow, n, data_size, &n_cb](rd_kafka_t* kafka, const rd_kafka_message_t* kafka_msg)
            {
                //std::cout << "cb thread_id: " << std::this_thread::get_id() << "\n";
                ++n_cb;
                //if ((n_cb) % 10000 == 0)
                //    std::cout << "n_cb: " << n_cb << "\n";
                if (n_cb == n) {
                    auto tend = std::chrono::system_clock::now();
                    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(tend - tnow).count();
                    std::cout << "cb len: " << data_size << " count: " << n
                        << " cost: " << delta << "ms\n";
                }
            });
    }
    auto tend = std::chrono::system_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(tend - tnow).count();
    std::cout << "len: " << data_size << " count: " << n
        << " cost: " << delta << "ms\n";

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
    return 0;
}

int main()
{
    auto conf = kafkacpp::Conf::create();
    conf->set("bootstrap.servers", "127.0.0.1:9092");
    if (!conf->set("request.required.acks", "all")) {
        std::cout << "set acks fail\n";
        return 0;
    }

    auto producer = kafkacpp::createProduer(std::move(conf));
    if (!producer->init()) {
        std::cout << "init fail\n";
        return 0;
    }

    auto topic = producer->createTopic("tp.test2");

    std::thread t{ [producer] 
        {
            std::cout << "thread_id: " << std::this_thread::get_id() << "\n";
            while (true) {
                producer->poll(10);
            }

        } };

    //testProducer(topic, producer);

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
