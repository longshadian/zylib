#include <chrono>
#include <iostream>
#include <set>

#include "zylib/Snowflake.h"

const uint64_t ID = 15;
const uint64_t SEQ = 7;

struct Cell
{
    int64_t m_tm;
    int64_t m_worker_id;
    int64_t m_seq;
};

Cell parseID(int64_t id)
{
    Cell c{};
    c.m_seq = (id & ((1<< SEQ)-1));

    id = id >> SEQ;
    c.m_worker_id = id & ((1<<ID) - 1);

    id = id >> ID;
    c.m_tm = id;
    return c;
}

int main()
{
    using SF = zylib::Snowflake<41, ID, SEQ>;
    SF sf{89, 0};
    std::cout << "tm_max:  " << int64_t(SF::MAX_TM) << "\n";
    std::cout << "id_max:  " << int64_t(SF::MAX_WORKER_ID) << "\n";
    std::cout << "seq_max: " << int64_t(SF::MAX_SEQUENCE) << "\n";

    int32_t n = 4096;
    std::set<int64_t> id_list{};
    while (true) {
        auto id = sf.nextID();
        auto it = id_list.insert(id);
        if (!it.second) {
            std::cout << "error:" << id << "\n";
            break;
        }
        --n;
        if (n == 0)
            break;
    }
    std::cout << id_list.size() << "\n";

    auto v = sf.nextID();

    auto c= parseID(v);
    std::cout << v << "\n";
    std::cout << c.m_tm << " " << c.m_worker_id << " " << c.m_seq << "\n";
    return 0;
}
