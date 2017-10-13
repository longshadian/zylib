#include "Kafkacpp.h"

#include <memory>
#include <thread>
#include <iostream>

int main()
{
    auto conf = std::make_shared<kafkacpp::Conf>();
    //conf->set("bootstrap.servers", "127.0.0.1:9092");
    conf->set("group.id", "xxx");
    std::string err_str{};
    auto consumer = conf->createConsumer(&err_str);
    consumer->addTopicAndPartition("tp.test", 0);
    if (!consumer->startConsume()) {
        std::cout << "start consume fail\n";
        return 0;
    }

    while (true) {
        consumer->poll(10);
        auto tnow = std::time(nullptr);
        std::cout << "tm: " << tnow << "\n";
        //std::this_thread::sleep_for(std::chrono::seconds{ 2 });
    }

    return 0;
}
