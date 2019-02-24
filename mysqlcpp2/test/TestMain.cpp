#include "test/TestMain.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <limits>

#define BOOST_TEST_MODULE MysqlcppTest
//#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp>

mysqlcpp::ConnectionOpt initConn()
{
    mysqlcpp::ConnectionOpt conn_opt{};
    conn_opt.user = "root";
    conn_opt.password = "123456";
    conn_opt.database = "mysqlcpp_test";
    conn_opt.host = "127.0.0.1";
    conn_opt.port = 3306;
    return conn_opt;
}

void CreateSchema() 
{
    const char* sql = 
        "CREATE SCHEMA `mysqlcpp_test` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci";
}

void testCreate_Schema_Table(PoolPtr pool)
{
    const char* sql_integer = 
        "   CREATE TABLE `mysqlcpp_test`.`test_integer` ("
        "   `fpk` INT NOT NULL,"
        "   `ftinyint` TINYINT(11) NULL,"
        "   `fsmallint` SMALLINT(11) NULL,"
        "   `fint` INT(11) NULL,"
        "   `fbigint` BIGINT(20) NULL,"
        "   `ftinyint_u` TINYINT(11) UNSIGNED NULL,"
        "   `fsmallint_u` SMALLINT(11) UNSIGNED NULL,"
        "   `fint_u` INT(11) UNSIGNED NULL,"
        "   `fbigint_u` BIGINT(20) UNSIGNED NULL,"
        "   PRIMARY KEY(`fpk`))"
        "   ENGINE = InnoDB"
        "   DEFAULT CHARACTER SET = utf8";

    const char* sql_float =
        "   CREATE TABLE `mysqlcpp_test`.`test_float` ("
        "   `fpk` INT NOT NULL,"
        "   `ffloat` FLOAT(10, 2) NULL,"
        "   `fdouble` DOUBLE(10, 4) NULL,"
        "   `fdecimal` DECIMAL(10, 6) NULL,"
        "   PRIMARY KEY(`fpk`))"
        "   ENGINE = InnoDB"
        "   DEFAULT CHARACTER SET = utf8";

    const char* sql_string =
        "   CREATE TABLE `mysqlcpp_test`.`test_string` ("
        "   `fpk` INT NOT NULL,"
        "   `fchar` CHAR(10) NULL,"
        "   `fvarchar` VARCHAR(10) NULL,"
        "   `ftext` TEXT NULL,"
        "   PRIMARY KEY(`fpk`))"
        "   ENGINE = InnoDB"
        "   DEFAULT CHARACTER SET = utf8";

    const char* sql_datetime = 
        "   CREATE TABLE `mysqlcpp_test`.`test_datetime` ("
        "   `fpk` INT NOT NULL,"
        "   `fdate` DATE NULL,"
        "   `ftime` TIME NULL,"
        "   `fdatetime` DATETIME NULL,"
        "   `ftimestamp` TIMESTAMP NULL,"
        "   PRIMARY KEY(`fpk`))"
        "   ENGINE = InnoDB"
        "   DEFAULT CHARACTER SET = utf8";

    const char* sql_binary =
        "   CREATE TABLE `mysqlcpp_test`.`test_binary` ("
        "   `fpk` INT NOT NULL,"
        "   `fblob` BLOB NULL,"
        "   `fvarbinary` VARBINARY(100) NULL,"
        "   PRIMARY KEY(`fpk`))"
        "   ENGINE = InnoDB"
        "   DEFAULT CHARACTER SET = utf8";

    mysqlcpp::ConnectionGuard conn{ *pool };
    auto stmt = conn->statement();
    //assert(stmt->execute("CREATE DATABASE IF NOT EXISTS mysqlcpp_test DEFAULT CHARACTER SET utf8;"));
    //assert(stmt->execute("use mysqlcpp_test;"));

    assert(stmt->execute("DROP TABLE IF EXISTS test_integer;"));
    assert(stmt->execute("DROP TABLE IF EXISTS test_string;"));
    assert(stmt->execute("DROP TABLE IF EXISTS test_float;"));
    assert(stmt->execute("DROP TABLE IF EXISTS test_datetime;"));
    assert(stmt->execute("DROP TABLE IF EXISTS test_binary;"));

    assert(stmt->execute(sql_integer));
    assert(stmt->execute(sql_float));
    assert(stmt->execute(sql_string));
    assert(stmt->execute(sql_datetime));
    assert(stmt->execute(sql_binary));
}


BOOST_AUTO_TEST_CASE(TestMain)
{
    int i = 1;
    BOOST_TEST(i);

    BOOST_TEST(i != 2);


}
