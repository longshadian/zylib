#ifndef _MYSQLCPP_MYSQLCONNECTION_H
#define _MYSQLCPP_MYSQLCONNECTION_H

#include <mysql.h>

#include "Types.h"

namespace mysqlcpp {

class DatabaseWorker;
class PreparedStatement;
class MySQLPreparedStatement;

struct MySQLConnectionInfo
{
    explicit MySQLConnectionInfo(std::string const& infoString)
    {
        /*
        Tokenizer tokens(infoString, ';');

        if (tokens.size() != 5)
            return;
            */

        /*
        uint8 i = 0;
        host.assign(tokens[i++]);
        port_or_socket.assign(tokens[i++]);
        user.assign(tokens[i++]);
        password.assign(tokens[i++]);
        database.assign(tokens[i++]);
        */
    }

    std::string user;
    std::string password;
    std::string database;
    std::string host;
    std::string port_or_socket;
};

class MySQLConnection
{
public:
    MySQLConnection(MySQLConnectionInfo& connInfo);
    virtual ~MySQLConnection();

    uint32 open();
    void close();

    MySQLPreparedStatementUPtr prepareStatement(const char* sql);
    PreparedResultSetPtr query(MySQLPreparedStatement& stmt);
    ResultSetPtr query(const char* sql);

    bool execute(const char* sql);
    bool execute(MySQLPreparedStatementUPtr& stmt);
    bool queryDetail(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount);
    bool queryDetail(MySQLPreparedStatement& stmt, MYSQL_RES **pResult, uint64* pRowCount, uint32* pFieldCount);

    void beginTransaction();
    void rollbackTransaction();
    void commitTransaction();

    operator bool () const { return m_mysql != NULL; }
    void Ping() { mysql_ping(m_mysql); }

    uint32 GetLastError() { return mysql_errno(m_mysql); }

    MYSQL* getMYSQL()  { return m_mysql; }
private:
    bool handleMySQLErrno(uint32 errNo, uint8 attempts = 5);
private:
    bool                    m_reconnecting;
    MYSQL*                  m_mysql;
    MySQLConnectionInfo&    m_conn_info;

    MySQLConnection(MySQLConnection const& right) = delete;
    MySQLConnection& operator=(MySQLConnection const& right) = delete;
};

}

#endif
