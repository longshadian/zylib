#include <iostream>
#include <type_traits>
#include <chrono>
#include <array>

#include "VarArray.h"
#include "TestTool.h"

const int32_t RECORD_NUM =  50;
const int32_t INVITE_NUM =  60;

const std::string REDIS_IP = "127.0.0.1";
const int REDIS_PORT = 6379;

std::shared_ptr<rediscpp::ConnectionPool> g_pool;
std::string SAVE_USER;

//#pragma pack(push, 1)

struct Record
{
    std::array<uint8_t, 50> m_data;
};

struct Invite
{
    std::array<uint8_t, 100> m_data;
};

struct Basic
{
    std::array<uint8_t, 500> m_data;
};

struct Obj
{
    std::vector<uint8_t> getRecordBinary() const
    {
        std::vector<uint8_t> val;
        m_records.serializeToBinary(&val);
        return val;
    }

    std::vector<uint8_t> getInviteBinary() const
    {
        std::vector<uint8_t> val;
        m_invites.serializeToBinary(&val);
        return val;
    }

    std::vector<uint8_t> getBasicBinary() const
    {
        std::vector<uint8_t> val;
        m_basic.serializeToBinary(&val);
        return val;
    }

    VarList<Record> m_records;
    VarList<Invite> m_invites;
    VarList<Basic>  m_basic;
};

//#pragma pack(pop)

bool initRedisPool()
{
    rediscpp::ConnectionOpt conn_opt{};
    conn_opt.m_ip = REDIS_IP;
    conn_opt.m_port = REDIS_PORT;
    g_pool = std::make_shared<rediscpp::ConnectionPool>(conn_opt);
    if (!g_pool->init()) {
        g_pool = nullptr;
        std::cout << "redis pool init fail\n";
        return false;
    }

    auto str = rediscpp::catFile("./user_save_user.lua");
    rediscpp::ConnectionGuard conn{ *g_pool };
    rediscpp::Script redis_script{*conn};
    auto ret = redis_script.LOAD(rediscpp::Buffer{ str });
    SAVE_USER = ret.asString();
    if (SAVE_USER.empty()) {
        return false;
    }
    return true;
}

bool saveUser(std::shared_ptr<Obj> obj, int32_t uid, std::time_t t)
{
    rediscpp::Buffer script_ret{};
    std::vector<rediscpp::Buffer> keys{};
    keys.reserve(2);
    keys.push_back(rediscpp::Buffer(uid));
    keys.push_back(rediscpp::Buffer(t));

    std::vector<rediscpp::Buffer> argv{};
    argv.reserve(6);
    argv.push_back(rediscpp::Buffer("tm"));
    argv.push_back(rediscpp::Buffer(t));

    argv.push_back(rediscpp::Buffer("record"));
    argv.push_back(rediscpp::Buffer(obj->getRecordBinary()));

    argv.push_back(rediscpp::Buffer("invite"));
    argv.push_back(rediscpp::Buffer(obj->getInviteBinary()));

    argv.push_back(rediscpp::Buffer("basic"));
    argv.push_back(rediscpp::Buffer(obj->getBasicBinary()));

    try {
        rediscpp::ConnectionGuard conn{ *g_pool };
        rediscpp::Script redis_script{ *conn };
        auto eval_return = redis_script.EVALSHA(rediscpp::Buffer{ SAVE_USER }
        , std::move(keys), std::move(argv));
        return eval_return.getBuffer().asInt32() == 1;
    } catch (const rediscpp::Exception& e) {
        std::cout << "rediscpp exception " << e.what() << "\n";
        return false;
    }
}

void testVar()
{
    using Type = VarArray<Record>;
    Type src{};
    Record r;
    test_tool::bzero(&r);
    int32_t cnt = 10;
    for (int32_t i = 0; i != cnt; ++i) {
        r.m_data.fill((uint8_t)i);
        src.m_data.push_back(r);
    }

    std::vector<uint8_t> src_bin{};
    src.serializeToBinary(&src_bin);

    Type dest{};
    if (!dest.parseFromBinary(src_bin.data(), src_bin.size())) {
        std::cout << "parseFromBinary error\n";
        return;
    }

    if (src.m_data.size() != dest.m_data.size()) {
        std::cout << "size error\n";
        return;
    }

    while (!src.m_data.empty()) {
        auto& src_val = src.m_data.back();
        auto& dest_val = dest.m_data.back();

        std::cout << "val " << (int)src_val.m_data[0] << " " << (int)dest_val.m_data[0] << "\n";
        if (src_val.m_data != dest_val.m_data) {
            std::cout << "src != dest " << (int)src_val.m_data[0] << " " << (int)dest_val.m_data[0] << "\n";
            return;
        }
        src.m_data.pop_back();
        dest.m_data.pop_back();
    }
}

int main()
{
    auto obj = std::make_shared<Obj>();
    for (int32_t i = 0; i != RECORD_NUM; ++i) {
        Record r;
        test_tool::bzero(&r);
        obj->m_records.m_data.push_back(r);
    }

    for (int32_t i = 0; i != INVITE_NUM; ++i) {
        Invite r;
        test_tool::bzero(&r);
        obj->m_invites.m_data.push_back(r);
    }

    for (int32_t i = 0; i != 1; ++i) {
        Basic r;
        test_tool::bzero(&r);
        obj->m_basic.m_data.push_back(r);
    }

    auto record_bin = obj->getRecordBinary();
    auto invite_bin = obj->getInviteBinary();
    auto basic_bin = obj->getBasicBinary();

    std::cout << "record size: " << record_bin.size() << "\n";
    std::cout << "invite size: " << invite_bin.size() << "\n";
    std::cout << "basic  size: " << basic_bin.size() << "\n";

    if (!initRedisPool())
        return 0;

    std::cout << SAVE_USER << "\n";

    test_tool::Timer timer{};
    for (int i = 0; i != 100; ++i) {
        saveUser(obj, 10 + i, 123 + i);
    }
    timer.end();
    std::cout << timer.cost() << "\n";

    testVar();

    return 0;
}
