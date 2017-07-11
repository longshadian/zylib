#ifndef _MYSQLCPP_CONNECTION_H
#define _MYSQLCPP_CONNECTION_H

#include <string>

#include <mysql.h>

#include "Types.h"

namespace mysqlcpp {

class PreparedStatement;
class Statement;

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
    friend class Statement;
    friend class PreparedStatement;

public:
    Connection(ConnectionOpt conn_opt);
    ~Connection();

    Connection(Connection const& right) = delete;
    Connection& operator=(Connection const& right) = delete;
public:
    uint32 open();
    void close();

    operator bool () const { return m_mysql != NULL; }
    int ping();
    MYSQL* getMYSQL();
    uint32 getErrno() const;
    const char* getError() const;

    StatementPtr createStatement();
    PreparedStatementPtr preparedStatement(const char* sql);
    PreparedStatementPtr preparedStatement(const std::string& sql);

private:
    void storeError(uint32 err_no, const char* err_str);
    PreparedStatementPtr createPreparedStatement(const char* sql, size_t sql_len);
private:
    MYSQL*          m_mysql;
    ConnectionOpt	m_conn_info;
    uint32          m_err_no;
    const char*     m_err_str;
    static const char m_null;
};

}

#endif
