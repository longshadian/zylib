#include "test/TestMain.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <limits>

#define BOOST_TEST_MODULE MysqlcppTest
#include <boost/test/unit_test.hpp>

const std::string g_database_name = "mysqlcpp_test";

const char* g_create_schema_sql =
    "CREATE SCHEMA `mysqlcpp_test` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci";

const char* g_create_table_sql =
    " CREATE TABLE `test` ( \
        `fpk` int(11) NOT NULL AUTO_INCREMENT, \
        `tinyint` tinyint(11) NOT NULL DEFAULT '0', \
        `smallint` smallint(11) NOT NULL DEFAULT '0', \
        `int` int(11) NOT NULL DEFAULT '0', \
        `bigint` bigint(20) NOT NULL DEFAULT '0', \
        `tinyint_u` tinyint(11) unsigned NOT NULL DEFAULT '0', \
        `smallint_u` smallint(11) unsigned NOT NULL DEFAULT '0', \
        `int_u` int(11) unsigned NOT NULL DEFAULT '0', \
        `bigint_u` bigint(20) unsigned NOT NULL DEFAULT '0', \
        `float` float(10, 2) DEFAULT NULL, \
        `double` double(10, 4) DEFAULT NULL, \
        `decimal` decimal(10, 6) DEFAULT NULL, \
        `char` char(10) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '', \
        `varchar` varchar(10) COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '', \
        `ftext` text COLLATE utf8mb4_unicode_ci, \
        `date` date NOT NULL DEFAULT '1970-01-01', \
        `time` time NOT NULL DEFAULT '08:00:00', \
        `datetime` datetime NOT NULL DEFAULT '1970-01-01 08:00:00', \
        `timestamp` timestamp NULL DEFAULT NULL, \
        PRIMARY KEY(`fpk`) \
        ) ENGINE = InnoDB DEFAULT CHARSET = utf8mb4 COLLATE = utf8mb4_unicode_ci; \
    ";

const char* g_delete_from_table = "DELETE FROM mysqlcpp_test";

const std::string& DatabaseName()
{
    return g_database_name;
}

std::shared_ptr<mysqlcpp::Connection> CreateConnection(std::string database_name)
{
    mysqlcpp::ConnectionOpt conn_opt{};
    conn_opt.user = "root";
    conn_opt.password = "123456";
    conn_opt.database = database_name;
    conn_opt.host = "192.168.32.128";
    conn_opt.port = 3306;
    auto conn = std::make_shared<mysqlcpp::Connection>(conn_opt);
    conn->Init();
    if (!conn->Open()) {
        std::cout << "conn open error: " << conn->GetErrorNo() << " " << conn->GetErrorStr() << "\n";
        return nullptr;
    }
    return conn;
}


/*
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
*/

bool DeleteFromTable()
{
    auto conn = CreateConnection();
    auto stmt = conn->CreateStatement();
    return stmt->Execute(g_delete_from_table);
}

BOOST_AUTO_TEST_CASE(TestMain)
{
    std::cout << "start test main\n";

    {
        auto conn = CreateConnection();
        BOOST_TEST(conn);
        auto stmt = conn->CreateStatement();
        BOOST_TEST(stmt);
        BOOST_REQUIRE(stmt->Execute(g_create_schema_sql));
        BOOST_REQUIRE(stmt->Execute("use " + DatabaseName()));
        BOOST_REQUIRE(stmt->Execute(g_create_table_sql));
    }

    int i = 1;
    BOOST_TEST(i);
}
