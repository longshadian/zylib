
#include <ctime>
#include <iostream>
#include <string>

#include "MySqlCpp.h"
#include "DataBaseService.h"


std::string fun(std::string s)
{
	std::cout << std::this_thread::get_id() << "\n";
	return s;
}

int test()
{
	DataBaseService dbservice{};
	if (!dbservice.init(3)) {
		std::cout << "error: db service init\n";
		return 0;
	}

	auto f1 = dbservice.asyncSubmit(std::bind(&fun, "1"));
	auto f2 = dbservice.asyncSubmit(std::bind(&fun, "2"));
	//auto f3 = dbservice.asyncSubmit(std::bind(&fun, "3"));
	auto f3 = dbservice.asyncSubmit([] { return fun("3");  });
	std::cout << f3.get() << "\n";
	for (size_t i = 10; i != 30; ++i) {
		dbservice.asyncSubmit(std::bind(&fun, std::to_string(i)));
	}

	dbservice.stop();
	dbservice.waitExit();
	std::cout << "service stop\n";
	dbservice.executeRemainTask();

	return 0;
}

int test2()
{
	DataBaseService dbservice{};
	if (!dbservice.init(3)) {
		std::cout << "error: db service init\n";
		return 0;
	}

	auto f1 = dbservice.countID(1);
	auto f2 = dbservice.countID(2);
	auto f3 = dbservice.countID(3);
	std::cout << f3.get() << "\n";
	for (size_t i = 10; i != 15; ++i) {
		dbservice.countID(i);
	}

	dbservice.stop();
	dbservice.waitExit();
	std::cout << "service stop\n";
	dbservice.executeRemainTask();

	return 0;
}

int testReconn()
{
	DataBaseService dbservice{};
	if (!dbservice.init(3)) {
		std::cout << "error: db service init\n";
		return 0;
	}

	auto f = dbservice.countID(20);
    std::cout << f.get() << "\n";
	dbservice.stop();
	dbservice.waitExit();
	std::cout << "service stop\n";
	dbservice.executeRemainTask();
	return 0;
}

void testConnClosed()
{
    mysqlcpp::ConnectionOpt conn_info{};
    conn_info.user = "root";
    conn_info.password = "123456";
    conn_info.database = "test";
    conn_info.host = "127.0.0.1";
    conn_info.port = 3306;
    conn_info.auto_reconn = true;

    mysqlcpp::Connection conn{ conn_info };
    if (conn.open() != 0) {
        std::cout <<"open error:" << conn.getErrno() << ":" << conn.getError() << "\n";
        return;
    }

    int n = 20;
    while (--n > 0) {
        std::cout << n << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    auto ret = conn.query("select fid from test.txx;");
    if (!ret) {
        std::cout <<"query error:" << conn.getErrno() << ":" << conn.getError() << "\n";
        return;
    }
    auto row = ret->getRow(0);
    std::cout << row["fid"]->getInt32() << "\n";
}


int main()
{
    mysqlcpp::initLog(&std::cout, mysqlcpp::LOG_ERROR);

    //test();
    //test2();
    //testReconn();
    testConnClosed();

    return 0;
}
