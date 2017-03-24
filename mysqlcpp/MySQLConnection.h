#ifndef _MYSQLCPP_MYSQLCONNECTION_H
#define _MYSQLCPP_MYSQLCONNECTION_H

#include <mysql.h>

#include "Types.h"

namespace mysqlcpp {

class MySQLPreparedStatement;

struct MySQLConnectionInfo
{
    std::string user{};
    std::string password{};
    std::string database{};
    std::string host{};
    uint32      port{3306};
};

class MySQLConnection
{
public:
    MySQLConnection(const MySQLConnectionInfo& conn_info);
    ~MySQLConnection();

    MySQLConnection(MySQLConnection const& right) = delete;
    MySQLConnection& operator=(MySQLConnection const& right) = delete;
public:

    uint32 open();
    void close();

    MySQLPreparedStatementUPtr prepareStatement(const char* sql);
    PreparedResultSetPtr query(MySQLPreparedStatement& stmt);
    ResultSetPtr query(const char* sql);

    bool execute(const char* sql);
    bool execute(MySQLPreparedStatementUPtr& stmt);
    bool queryDetail(const char *sql);
    bool queryDetail(MySQLPreparedStatement& stmt);

    void beginTransaction();
    void rollbackTransaction();
    void commitTransaction();

    operator bool () const { return m_mysql != NULL; }
    void ping() { ::mysql_ping(m_mysql); }
    MYSQL* getMYSQL()  { return m_mysql; }

    uint32 getErrno() const;
    const char* getError() const;
private:
    bool handleMySQLErrno(uint32 err_no, uint8 attempts = 5);
private:
    bool                        m_reconnecting;
    MYSQL*                      m_mysql;
    const MySQLConnectionInfo&  m_conn_info;
};

}

#endif
