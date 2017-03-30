
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
	dbservice.waitStop();
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
	for (size_t i = 10; i != 30; ++i) {
		dbservice.countID(i);
	}

	dbservice.stop();
	dbservice.waitStop();
	std::cout << "service stop\n";
	dbservice.executeRemainTask();

	return 0;
}


int main()
{
    //test();
	test2();

    return 0;
}
