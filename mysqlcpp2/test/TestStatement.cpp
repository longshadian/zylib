#include "mysqlcpp/mysqlcpp.h"
#include "test/TestMain.h"


//#include <boost/test/unit_test.hpp>
BOOST_AUTO_TEST_CASE(TestStatement)
{
    BOOST_REQUIRE(DeleteFromTable());
    auto conn = CreateConnection(DatabaseName());
    BOOST_REQUIRE(conn);
    auto stmt = conn->CreateStatement();
    BOOST_REQUIRE(stmt);

    assert(stmt->execute(sql));
    sql = "INSERT INTO `test_integer` (`fpk`, `fint`, `fbigint`) "
        " VALUES (1, 100, 100000), (2, 100, 100000)";
    {
        assert(stmt->execute(sql));
        sql = "SELECT `fint`, `fbigint` FROM `test_integer` where fpk=1";
        auto rs = stmt->executeQuery(sql);
        assert(rs);
        auto row = rs->getRow(0);
        assert(row["fint"]->getInt32() == 100);
        assert(row["fbigint"]->getInt64() == 100000);
    }

    //测试int类型
    {
        sql = "DELETE FROM `test_integer`";
        assert(stmt->execute(sql));

        sql = " INSERT INTO `test_integer` "
            " (fpk, ftinyint, fsmallint, fint, fbigint,"
            " ftinyint_u, fsmallint_u, fint_u, fbigint_u)"
            "VALUES "
            " (?, ?, ?, ?, ?,"
            " ?,?,?,?)";
        auto ps = conn->preparedStatement(sql);
        assert(ps);
        ps->setInt32(0, 1);

        ps->setInt8(1, 127);
        ps->setInt16(2, 32767);
        ps->setInt32(3, 2147483647);
        ps->setInt64(4, 9223372036854775807u);

        ps->setUInt8(5, 127);
        ps->setUInt16(6, 32767);
        ps->setUInt32(7, 4294967295);
        ps->setUInt64(8, 18446744073709551615u);
        assert(ps->execute());

        sql =
            "SELECT ftinyint, fsmallint, fint, fbigint,"
            " ftinyint_u, fsmallint_u, fint_u, fbigint_u "
            " FROM `test_integer` where fpk=?";
        ps = conn->preparedStatement(sql);
        assert(ps);
        ps->setInt32(0, 1);
        auto rs = ps->executeQuery();
        assert(rs);

        auto row = rs->getRow(0);
        assert(row["ftinyint"]->getInt8() == 127);
        assert(row["fsmallint"]->getInt16() == 32767);
        assert(row["fint"]->getInt32() == 2147483647);
        assert(row["fbigint"]->getInt64() == 9223372036854775807);

        assert(row["ftinyint_u"]->getUInt8() == 127);
        assert(row["fsmallint_u"]->getUInt16() == 32767);
        assert(row["fint_u"]->getUInt32() == 4294967295);
        assert(row["fbigint_u"]->getUInt64() == 18446744073709551615u);

        ps->clearParameters();
        ps->setInt32(0, 1000);
        rs = ps->executeQuery();
        assert(rs);
        assert(rs->getRowCount() == 0);
    }

    {
        //测试string类型
        sql = "DELETE FROM `test_string`";
        assert(stmt->execute(sql));

        sql = "INSERT INTO `test_string` (fpk, fchar, fvarchar, ftext) VALUES "
            " (1, 'aaa', 'bbb', 'qwertyuiop')";
        assert(stmt->execute(sql));

        sql = "SELECT `fchar`, `fvarchar`, `ftext` FROM `test_string` where fpk=?";
        auto ps = conn->preparedStatement(sql);
        ps->setInt32(0, 1);
        auto rs = ps->executeQuery();
        assert(rs);
        auto row = rs->getRow(0);
        assert(row["fchar"]->getString() == "aaa");
        assert(row["fvarchar"]->getString() == "bbb");
        assert(row["ftext"]->getString() == "qwertyuiop");

        /*
        std::cout << "===================\n";
        auto ftext = row["ftext"]->getString();
        for (size_t i = 0; i != ftext.size(); ++i) {
            std::cout << ftext[i] << " ";
        }
        std::cout << "\n";
        const auto& internal_buff = row["ftext"]->getInternalBuffer();
        std::cout << "capacity " << internal_buff.getCapacity() << "\n";
        std::cout << "length   " << internal_buff.getLength() << "\n";
        std::cout << "bin size " << internal_buff.getBinary().size() << "\n";
        std::cout << "cstring  " << internal_buff.getCString() << "\n";
        std::cout << "===================\n";
        */
    }

    {
        //测试float类型
        sql = "DELETE FROM `test_float`";
        assert(stmt->execute(sql));

        sql = "INSERT INTO `test_float` (fpk, ffloat, fdouble) VALUES "
            " (1, 12345, 12345.6789)";
        assert(stmt->execute(sql));

        sql = "SELECT `ffloat`,`fdouble`  FROM `test_float` where fpk=?";
        auto ps = conn->preparedStatement(sql);
        ps->setInt32(0, 1);
        auto rs = ps->executeQuery();
        assert(rs);
        auto row = rs->getRow(0);
        assert(row["ffloat"]->getFloat() == 12345.f);
        assert(row["fdouble"]->getDouble() == double(12345.6789));
    }

    {
        //测试datetime类型
        sql = "DELETE FROM `test_datetime`";
        auto stmt_ex = conn->statement();
        assert(stmt_ex->execute(sql));

        time_t tnow = 1475791730;   //2016-10-07 06:08:50
        sql = " INSERT INTO `test_datetime` "
            " (fpk, fdate, ftime, fdatetime, ftimestamp) VALUES "
            " (1, ?, ?, ?, ?)";
        auto ps = conn->preparedStatement(sql);
        assert(ps);
        ps->setDateTime(0, mysqlcpp::DateTime(tnow));     //保存date类型
        ps->setDateTime(1, mysqlcpp::DateTime(tnow));     //保存time类型
        ps->setDateTime(2, mysqlcpp::DateTime(tnow));     //保存datetime类型
        ps->setNull(3);                                   //保存datetime类型

        assert(ps->execute());

        sql = "SELECT `fdate`, `ftime`,`fdatetime`,`ftimestamp`  FROM `test_datetime` where fpk=1";
        auto rs = stmt->executeQuery(sql);
        assert(rs);
        auto row = rs->getRow(0);

        //assert(row["fdate"]->getDateTime().getString() == "2016-10-07");        //date类型
        //assert(row["ftime"]->getDateTime().getString() == "06:08:50");          //time类型
        assert(row["fdatetime"]->getDateTime().getString() == "2016-10-07 06:08:50");   //datetime类型
        assert(row["fdatetime"]->getDateTime().getTime() == tnow);                      //获取time_t
        assert(row["ftimestamp"]->isNull());  // 默认值
    }

    {
        //测试binary类型
        sql = "DELETE FROM `test_binary`";
        auto stmt_ex = conn->statement();
        assert(stmt_ex->execute(sql));

        sql = " INSERT INTO `test_binary` "
            " (fpk, fblob, fvarbinary) VALUES "
            " (?, ?, ?)";
        auto ps = conn->preparedStatement(sql);
        assert(ps);

        std::vector<uint8_t> vals{};
        for (int32_t i = 0; i != 100; ++i) {
            vals.push_back(std::rand() % std::numeric_limits<uint8_t>::max());
        }

        std::string str_bin = "123456aoisd0f9820 398m0fas9 d23 sdfn2-3904usafdhgp98239hsifgh09825h";
        std::vector<uint8_t> str_bin_buffer{};
        for (auto c : str_bin) {
            str_bin_buffer.push_back(static_cast<uint8_t>(c));
        }

        ps->setInt32(0, 1);
        ps->setBinary(1, vals, false);
        ps->setBinary(2, str_bin_buffer, false);
        assert(ps->execute());

        sql = "SELECT fblob, fvarbinary from test_binary where fpk = ?";
        ps = conn->preparedStatement(sql);
        assert(ps);
        ps->setInt32(0, 1);
        auto rs = ps->executeQuery();
        assert(rs);
        auto row = rs->getRow(0);
        auto blob_bin = row["fblob"]->getBinary();
        auto varbinary_bin = row["fvarbinary"]->getBinary();
        assert(vals == blob_bin);
        assert(str_bin_buffer == varbinary_bin);
    }

    std::cout << __FUNCTION__ << " " << conn.get() << " success\n";
}

void testUpdate(mysqlcpp::ConnectionGuard& conn)
{
    const char* sql = "delete from test_integer";
    auto ps = conn->preparedStatement(sql);
    assert(ps);
    assert(ps->execute());

    sql = "INSERT INTO `test_integer` (`fpk`, `fint`, `fbigint`) "
        " VALUES (?, ?, ?)";
    ps = conn->preparedStatement(sql);
    for (int i = 0; i != 10; ++i) {
        ps->clearParameters();
        ps->setInt32(0, i + 10);
        ps->setInt32(1, i + 100);
        ps->setInt32(2, i + 1000);
        assert(ps->execute());
    }

    sql = "select fpk as xxx, fint, fbigint from test_integer where ? <= fpk and fpk <= ? order by fpk";
    ps = conn->preparedStatement(sql);
    assert(ps);
    ps->setInt32(0, 1);
    ps->setInt64(1, 8);
    auto rs = ps->executeQuery();
    assert(rs);
    for (uint64_t i = 0; i != rs->getRowCount(); ++i) {
        auto row = rs->getRow(i);
        auto fpk = row["xxx"]->getInt32();
        auto fint = row["fint"]->getInt32();
        auto fbigint = row["fbigint"]->getInt64();
        assert(fpk == int32_t(10 + i));
        assert(fint == int32_t(fpk + 100));
        assert(fbigint == fpk + 1000);
    }

    sql = "update test_integer set fint =?, fbigint=? where fpk = ?";
    ps = conn->preparedStatement(sql);
    assert(ps);
    for (int i = 0; i != 30; ++i) {
        ps->clearParameters();
        ps->setInt32(0, i);
        ps->setInt32(1, i*2);
        ps->setInt32(2, i);

        assert(ps->execute());
        //std::cout << "fpk:" << i << " " << ps->getAffectedRows() << "\n";
    }
    std::cout << __FUNCTION__ << " " << conn.get() << " success\n";
}

void testTransaction(mysqlcpp::ConnectionGuard& conn)
{
    const char* sql = "delete from test_integer;";
    auto ps = conn->preparedStatement(sql);
    assert(ps);
    assert(ps->execute());

    auto stmt = conn->statement();
    assert(stmt->execute("INSERT INTO `test_integer` (`fpk`, `fint`, `fbigint`) VALUES (10, 10, 10)"));
    do {
        mysqlcpp::Transaction trans{*conn};

        sql = "INSERT INTO `test_integer` (`fpk`, `fint`, `fbigint`) VALUES (?, ?, ?)";
        ps = conn->preparedStatement(sql);
        ps->setInt32(0, 1);
        ps->setInt32(1, 100);
        ps->setInt64(2, 1000);
        assert(ps->execute());

        sql = "INSERT INTO `test_integer` (`fpk`, `fint`, `fbigint`) VALUES (?, ?, ?)";
        ps = conn->preparedStatement(sql);
        if (ps) {
            ps->setInt32(0, 2);
            ps->setInt32(1, 200);
            ps->setInt64(2, 2000);
            if (ps->execute()) {
                trans.commit();
                std::cout << "transaction commit\n";
            }
        }
    } while(0);

    sql = "update test_integer set fint = ?, fbigint = ? where  fpk = ?";
    ps = conn->preparedStatement(sql);
    assert(ps);
    ps->setInt32(0, 9);
    ps->setInt64(1, 9999);
    ps->setInt32(2, 10);
    assert(ps->execute());

    /*
    int n = 10;
    while (n>0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << --n << "\n";
    }
    */
    std::cout << __FUNCTION__ << " " << conn.get() << " success\n";
}

/*
int main()
{
    std::srand((unsigned int)std::time(nullptr));

    std::cout << "int16_t  max " << std::numeric_limits<int16_t>::max() << "\n";
    std::cout << "int32_t  max " << std::numeric_limits<int32_t>::max() << "\n";
    std::cout << "int64_t  max " << std::numeric_limits<int64_t>::max() << "\n";

    std::cout << "uint16_t max " << std::numeric_limits<uint16_t>::max() << "\n";
    std::cout << "uint32_t max " << std::numeric_limits<uint32_t>::max() << "\n";
    std::cout << "uint64_t max " << std::numeric_limits<uint64_t>::max() << "\n";

    mysqlcpp::initLog(std::make_unique<mysqlcpp::LogStreamConsole>());
    auto pool = initPool();
    if (!pool)
        return 0;

    testCreate_Schema_Table(pool);

    for (int32_t i = 0; i != POOL_SIZE; ++i) {
        mysqlcpp::ConnectionGuard conn{ *pool };
        testBasic(conn);
    }

    mysqlcpp::ConnectionGuard conn{ *pool };
    testUpdate(conn);
    mysqlcpp::ConnectionGuard conn_1{ *pool };
    testTransaction(conn_1);

    return 0;
}
*/
