#pragma once

#include <string>

#include "mysqlcpp/mysqlcpp.h"
#include <boost/test/unit_test.hpp>

const std::string& DatabaseName();
std::shared_ptr<mysqlcpp::Connection> CreateConnection(std::string database_name = "");

bool DeleteFromTable();

