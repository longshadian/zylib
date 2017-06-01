
#include <cassert>
#include <ctime>
#include <iostream>
#include <string>
#include <memory>
#include <sys/time.h>

#include "MySqlCpp.h"

#define ASSERT assert

mysqlcpp::ConnectionOpt initConn()
{
    mysqlcpp::ConnectionOpt conn_info{};
    conn_info.user = "root";
    conn_info.password = "123456";
    //conn_info.database = "mytest";
    conn_info.host = "192.168.0.123";
    conn_info.port = 3306;
    return conn_info;
}

void test()
{
    auto conn_info = initConn();
    mysqlcpp::Connection conn{conn_info};
    if (conn.open() != 0) {
        std::cout << "open error\n";
        return;
    }
    std::shared_ptr<mysqlcpp::ResultSet> ret;
    std::shared_ptr<mysqlcpp::PreparedStatement> stmt;
    const char* sql = nullptr;

    ret = conn.query("use mj_game;");
    ASSERT(ret && ret->getRowCount() == 0);

    sql = "INSERT INTO `user_zj_records` "
        " (`user_id`, `record_id`) "
        " VALUES "
        " (?, ?), (?, ?), (?, ?), (?, ?)";

    //mysqlcpp::Transaction transaction{conn};

    stmt = conn.prepareStmt(sql);
    ASSERT(stmt);

    int32_t record_id = 999;

    stmt->setInt64(0, 100);
    stmt->setInt64(1, record_id);

    stmt->setInt64(2, 101);
    stmt->setInt64(3, record_id);

    stmt->setInt64(4, 102);
    stmt->setInt64(5, record_id);

    stmt->setInt64(6, 103);
    stmt->setInt64(7, record_id);

    auto stmt_ret = conn.execute(*stmt);
    if (!stmt_ret) {
        std::cout << "ERROR:exe fail:" << conn.getErrno() << " " << conn.getError() << "\n";
        return;
    }
    //transaction.commit();
    std::cout << "insert success\n";
}

int main()
{
    mysqlcpp::initLog(&std::cout);
    test();

    return 0;
}
