
#include "MySQLConnection.h"

#include <mysql.h>
#include <errmsg.h>

#include "QueryResult.h"
#include "PreparedStatement.h"
#include "FakeLog.h"
#include "Utils.h"

namespace mysqlcpp {

MySQLConnection::MySQLConnection(MySQLConnectionInfo& connInfo) 
    : m_reconnecting(false)
    , m_mysql(nullptr)
    , m_conn_info(connInfo)
{
}

MySQLConnection::~MySQLConnection()
{
    close();
}

void MySQLConnection::close()
{
    if (m_mysql) {
        ::mysql_close(m_mysql);
        m_mysql = nullptr;
    }
}

PreparedResultSetPtr MySQLConnection::query(MySQLPreparedStatement& stmt)
{
    MYSQL_RES* result = nullptr;
    uint64 row_count = 0;
    uint32 field_count = 0;

    if (!queryDetail(stmt, &result, &row_count, &field_count))
        return nullptr;

    if (::mysql_more_results(m_mysql)) {
        ::mysql_next_result(m_mysql);
    }
    return std::make_shared<PreparedResultSet>(stmt.getMYSQL_STMT(), result, row_count, field_count);
}

ResultSetPtr MySQLConnection::query(const char* sql)
{
    if (!sql)
        return nullptr;

    MYSQL_RES *result = nullptr;
    MYSQL_FIELD *fields = nullptr;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    if (!queryDetail(sql, &result, &fields, &rowCount, &fieldCount))
        return nullptr;

    return std::make_shared<ResultSet>(result, fields, rowCount, fieldCount);
}

uint32 MySQLConnection::open()
{
    MYSQL* mysql;
    mysql = ::mysql_init(nullptr);
    if (!mysql) {
        FAKE_LOG_ERROR() << "Could not initialize Mysql connection to database " << m_conn_info.database;
        return CR_UNKNOWN_ERROR;
    }

    int port;
    char const* unix_socket;
    //unsigned int timeout = 10;

    ::mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8");
    //::mysql_options(mysqlInit, MYSQL_OPT_READ_TIMEOUT, (char const*)&timeout);

    if (m_conn_info.host == ".") {
        unsigned int opt = MYSQL_PROTOCOL_SOCKET;
        ::mysql_options(mysql, MYSQL_OPT_PROTOCOL, (char const*)&opt);
        m_conn_info.host = "localhost";
        port = 0;
        unix_socket = m_conn_info.port_or_socket.c_str();
    } else {
        port = atoi(m_conn_info.port_or_socket.c_str());
        unix_socket = nullptr;
    }

    m_mysql = ::mysql_real_connect(mysql, m_conn_info.host.c_str(), m_conn_info.user.c_str(),
        m_conn_info.password.c_str(), m_conn_info.database.c_str(), port, unix_socket, 0);

    if (m_mysql) {
        if (!m_reconnecting) {
            FAKE_LOG_INFO() << "MySQL client library:" << ::mysql_get_client_info();
            FAKE_LOG_INFO() << "MySQL server ver: " << ::mysql_get_server_info(m_mysql);
        }

        FAKE_LOG_INFO() << "Connected to MySQL database at " << m_conn_info.host;
        ::mysql_autocommit(m_mysql, 1);

        // set connection properties to UTF8 to properly handle locales for different
        // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
        ::mysql_set_character_set(m_mysql, "utf8");
        return 0;
    } else {
        FAKE_LOG_ERROR() << "Could not connect to MySQL database at " << m_conn_info.host.c_str() << " : " << ::mysql_error(mysql);
        ::mysql_close(mysql);
        return ::mysql_errno(mysql);
    }
}

bool MySQLConnection::execute(const char* sql)
{
    if (!m_mysql)
        return false;

    if (::mysql_query(m_mysql, sql)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        FAKE_LOG_ERROR() << "mysql_query " << err_no << ":" << ::mysql_error(m_mysql);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return execute(sql);       // Try again
        return false;
    }
    return true;
}

bool MySQLConnection::execute(MySQLPreparedStatementUPtr& stmt)
{
    if (!m_mysql)
        return false;
    MYSQL_STMT* msql_stmt = stmt->getMYSQL_STMT();
    MYSQL_BIND* msql_bind = stmt->getMYSQL_BIND();

    if (::mysql_stmt_bind_param(msql_stmt, msql_bind)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        FAKE_LOG_ERROR() << "mysql_stmt_bind_param " << err_no << ":" << ::mysql_stmt_error(msql_stmt);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return execute(stmt);       // Try again
        return false;
    }

    if (::mysql_stmt_execute(msql_stmt)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        FAKE_LOG_ERROR() << "mysql_stmt_execute " << err_no << ":" << ::mysql_stmt_error(msql_stmt);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return execute(stmt);       // Try again
        return false;
    }
    return true;
}

bool MySQLConnection::queryDetail(MySQLPreparedStatement& stmt, MYSQL_RES **pResult, uint64* pRowCount, uint32* pFieldCount)
{
    if (!m_mysql)
        return false;

    MYSQL_STMT* msql_stmt = stmt.getMYSQL_STMT();
    MYSQL_BIND* msql_bind = stmt.getMYSQL_BIND();
    if (::mysql_stmt_bind_param(msql_stmt, msql_bind)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        FAKE_LOG_ERROR() << "mysql_stmt_bind_param " << err_no << ":" << ::mysql_stmt_error(msql_stmt);
        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return queryDetail(stmt, pResult, pRowCount, pFieldCount);       // Try again
        return false;
    }

    if (::mysql_stmt_execute(msql_stmt)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        FAKE_LOG_ERROR() << "mysql_stmt_execute " << err_no << ":" << ::mysql_stmt_error(msql_stmt);
        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return queryDetail(stmt, pResult, pRowCount, pFieldCount);      // Try again
        return false;
    }

    *pResult    = ::mysql_stmt_result_metadata(msql_stmt);
    *pRowCount  = ::mysql_stmt_num_rows(msql_stmt);
    *pFieldCount = ::mysql_stmt_field_count(msql_stmt);
    return true;
}

bool MySQLConnection::queryDetail(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount)
{
    if (!m_mysql)
        return false;

    if (::mysql_query(m_mysql, sql)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        FAKE_LOG_ERROR() << "mysql_query " << err_no << ":" << ::mysql_error(m_mysql);
        if (handleMySQLErrno(err_no))      // If it returns true, an error was handled successfully (i.e. reconnection)
            return queryDetail(sql, pResult, pFields, pRowCount, pFieldCount);    // We try again
        return false;
    }

    *pResult = ::mysql_store_result(m_mysql);
    *pRowCount = ::mysql_affected_rows(m_mysql);
    *pFieldCount = ::mysql_field_count(m_mysql);

    if (!*pResult)
        return false;

    if (!*pRowCount) {
        ::mysql_free_result(*pResult);
        return false;
    }
    *pFields = ::mysql_fetch_fields(*pResult);
    return true;
}

void MySQLConnection::beginTransaction()
{
    execute("START TRANSACTION");
}

void MySQLConnection::rollbackTransaction()
{
    execute("ROLLBACK");
}

void MySQLConnection::commitTransaction()
{
    execute("COMMIT");
}

MySQLPreparedStatementUPtr MySQLConnection::prepareStatement(const char* sql)
{
    MYSQL_STMT* stmt = ::mysql_stmt_init(m_mysql);
    if (!stmt) {
        FAKE_LOG_ERROR() << "mysql_stmt_init " << ::mysql_error(m_mysql);
        return nullptr;
    }

    if (::mysql_stmt_prepare(stmt, sql, static_cast<unsigned long>(std::strlen(sql)))) {
        FAKE_LOG_ERROR() << "mysql_stmt_prepare " << ::mysql_stmt_error(stmt);
        ::mysql_stmt_close(stmt);
        return nullptr;
    }
    return util::make_unique<MySQLPreparedStatement>(stmt);
}

bool MySQLConnection::handleMySQLErrno(uint32 err_no, uint8 attempts /*= 5*/)
{
    switch (err_no) {
    case CR_SERVER_GONE_ERROR:
    case CR_SERVER_LOST:
    case CR_INVALID_CONN_HANDLE:
    case CR_SERVER_LOST_EXTENDED: {
        if (m_mysql) {
            FAKE_LOG_ERROR() << "Lost the connection to the MySQL server!";
            ::mysql_close(getMYSQL());
            m_mysql = nullptr;
        }
        /*no break*/
    }
    case CR_CONN_HOST_ERROR: {
        FAKE_LOG_INFO() << "Attempting to reconnect to the MySQL server...";
        m_reconnecting = true;
        const uint32 err_no_ex = open();
        if (!err_no_ex) {
            FAKE_LOG_INFO() << "Successfully reconnected to " << m_conn_info.database 
                << " " << m_conn_info.host << " " << m_conn_info.port_or_socket;
            m_reconnecting = false;
            return true;
        }

        if ((--attempts) == 0) {
            // Shut down the server when the mysql server isn't
            // reachable for some time
            FAKE_LOG_ERROR() << "Failed to reconnect to the MySQL server,terminating the server to prevent data corruption!";
            return false;
        } else {
            // It's possible this attempted reconnect throws 2006 at us.
            // To prevent crazy recursive calls, sleep here.
            //std::this_thread::sleep_for(std::chrono::seconds(3)); // Sleep 3 seconds
            return handleMySQLErrno(err_no_ex, attempts); // Call self (recursive)
        }
    }

                             /*
    case ER_LOCK_DEADLOCK:
            return false;    // Implemented in TransactionTask::Execute and DatabaseWorkerPool<T>::DirectCommitTransaction
    // Query related errors - skip query
    case ER_WRONG_VALUE_COUNT:
    case ER_DUP_ENTRY:
        return false;

    // Outdated table or database structure - terminate core
    case ER_BAD_FIELD_ERROR:
    case ER_NO_SUCH_TABLE:
        FAKE_LOG_ERROR() << "Your database structure is not up to date. Please make sure you've executed all queries in the sql/updates folders.";
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::abort();
        return false;
    case ER_PARSE_ERROR:
        FAKE_LOG_ERROR() << "Error while parsing SQL. Core fix required.";
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::abort();
        return false;
        */
    default:
        FAKE_LOG_ERROR() <<  "Unhandled MySQL errno:" << err_no << " Unexpected behaviour possible.";
        return false;
    }
}

}
