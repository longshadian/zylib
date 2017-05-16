#ifndef _MYSQLCPP_CONNECTION_H
#define _MYSQLCPP_CONNECTION_H

#include <mysql.h>

#include "Types.h"

namespace mysqlcpp {

class PreparedStatement;

struct ConnectionOpt
{
	ConnectionOpt() = default;
	~ConnectionOpt() = default;
	ConnectionOpt(const ConnectionOpt& rhs) = default;
	ConnectionOpt& operator=(const ConnectionOpt& rhs) = default;
	ConnectionOpt(ConnectionOpt&& rhs) = default;
	ConnectionOpt& operator=(ConnectionOpt&& rhs) = default;

    std::string user{};
    std::string password{};
    std::string database{};
    std::string host{};
    uint32      port{3306};
    bool        auto_reconn{false};
};

class Connection
{
public:
    Connection(ConnectionOpt conn_opt);
    ~Connection();

    Connection(Connection const& right) = delete;
    Connection& operator=(Connection const& right) = delete;
public:
    uint32 open();
    void close();

    PreparedStatementUPtr prepareStmt(const char* sql);
    PreparedResultSetPtr query(PreparedStatement& stmt);
    ResultSetPtr query(const char* sql);

    bool execute(const char* sql);
    bool execute(PreparedStatement& stmt);

    void beginTransaction();
    void rollbackTransaction();
    void commitTransaction();

    operator bool () const { return m_mysql != NULL; }
    void ping();
    MYSQL* getMYSQL();
    uint32 getErrno() const;
    const char* getError() const;
private:
    bool handleMySQLErrno(uint32 err_no);
    bool queryDetail(const char *sql);
    bool queryDetail(PreparedStatement& stmt);

    void storeError(uint32 err_no, const char* err_str);
private:
    MYSQL*          m_mysql;
    ConnectionOpt	m_conn_info;
    uint32          m_err_no;
    const char*     m_err_str;
    static const char m_null;
};

}

#endif
