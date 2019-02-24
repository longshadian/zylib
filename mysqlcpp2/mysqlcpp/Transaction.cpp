#include "mysqlcpp/Transaction.h"

#include <algorithm>

#include "mysqlcpp/Connection.h"
#include "mysqlcpp/Statement.h"

namespace mysqlcpp {

Transaction::Transaction(Connection& conn)
    : m_conn(conn)
    , m_stmt(conn.CreateStatement())
    , m_rollback(true)
{
    m_stmt->Execute("START TRANSACTION");
}

Transaction::~Transaction()
{
    if (m_rollback) {
        m_stmt->Execute("ROLLBACK");
    }
}

void Transaction::Commit()
{
    m_stmt->Execute("COMMIT");
    m_rollback = false;
}


}
