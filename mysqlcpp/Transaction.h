#ifndef _MYSQLCPP_TRANSACTION_H
#define _MYSQLCPP_TRANSACTION_H

#include <memory>
#include <mysql.h>

#include "Assert.h"

namespace mysqlcpp {

class MySQLConnection;

class Transaction
{
public:
    Transaction(MySQLConnection& conn);
    ~Transaction();
    Transaction(const Transaction& rhs) = delete;
    Transaction& operator=(const Transaction& rhs) = delete;

    void commit();
private:
    MySQLConnection& m_conn;
    bool             m_rollback;
};

}

#endif

