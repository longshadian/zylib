#include "Connection.h"

#include <mysql.h>
#include <errmsg.h>

#include "QueryResult.h"
#include "PreparedStatement.h"
#include "FakeLog.h"
#include "Utils.h"

namespace mysqlcpp {

const char Connection::m_null = '\0';

Connection::Connection(ConnectionOpt conn_opt) 
	: m_mysql(nullptr)
    , m_conn_info(conn_opt)
    , m_err_no(0)
    , m_err_str(nullptr)
{
}

Connection::~Connection()
{
    close();
}

void Connection::close()
{
    if (m_mysql) {
        ::mysql_close(m_mysql);
        m_mysql = nullptr;
    }
}

PreparedResultSetPtr Connection::query(PreparedStatement& stmt)
{
    if (!queryDetail(stmt))
        return nullptr;
    if (::mysql_more_results(m_mysql)) {
        ::mysql_next_result(m_mysql);
    }
    auto result = std::make_shared<PreparedResultSet>(*m_mysql, *stmt.getMYSQL_STMT());
    if (!result->init()) {
        return nullptr;
    }
    return result;
}

ResultSetPtr Connection::query(const char* sql)
{
    if (!sql)
        return nullptr;
    if (!queryDetail(sql))
        return nullptr;

    auto result = std::make_shared<ResultSet>(*m_mysql);
    if (!result->init()) {
        return nullptr;
    }
    return result;
}

uint32 Connection::open()
{
	if (m_mysql) {
        storeError(CR_UNKNOWN_ERROR, nullptr);
        return m_err_no;
    }

	m_mysql = ::mysql_init(nullptr);
    if (!m_mysql) {
        FAKE_LOG(ERROR) << "Could not initialize Mysql connection to database " << m_conn_info.database;
        storeError(CR_UNKNOWN_ERROR, nullptr);
        return m_err_no;
    }

    ::mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, "utf8");

    //unsigned int timeout = 10;
    //::mysql_options(mysqlInit, MYSQL_OPT_READ_TIMEOUT, (char const*)&timeout);
    const char* host = m_conn_info.host.c_str();
    const char* user = m_conn_info.user.c_str();
    const char* passwd = m_conn_info.password.c_str();
    const char* db = nullptr;
    if (!m_conn_info.database.empty())
        db = m_conn_info.database.c_str();

    m_mysql = ::mysql_real_connect(m_mysql, host, user, passwd, db, m_conn_info.port, nullptr, 0);

    if (m_mysql) {
		FAKE_LOG(INFO) << "MySQL client library:" << ::mysql_get_client_info();
		FAKE_LOG(INFO) << "MySQL server ver: " << ::mysql_get_server_info(m_mysql);
        FAKE_LOG(INFO) << "Connected to MySQL database at " << m_conn_info.host;
        ::mysql_autocommit(m_mysql, 1);

        // set connection properties to UTF8 to properly handle locales for different
        // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
        ::mysql_set_character_set(m_mysql, "utf8");
        return 0;
    } else {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_error(m_mysql);
        FAKE_LOG(ERROR) << "Could not connect to MySQL database at " << m_conn_info.host.c_str() << " " << err_no << " : " << err_str;
        storeError(err_no, err_str);
        return m_err_no;
    }
}

bool Connection::execute(const char* sql)
{
    if (!m_mysql) {
        storeError(CR_UNKNOWN_ERROR, nullptr);
        return false;
    }
    if (::mysql_query(m_mysql, sql)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_error(m_mysql);
        FAKE_LOG(ERROR) << "mysql_query " << err_no << ":" << err_str;
        storeError(err_no, err_str);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return execute(sql);       // Try again
        return false;
    }
    return true;
}

bool Connection::execute(PreparedStatementUPtr& stmt)
{
    if (!m_mysql) {
        storeError(CR_UNKNOWN_ERROR, nullptr);
        return false;
    }
    MYSQL_STMT* mysql_stmt = stmt->getMYSQL_STMT();
    MYSQL_BIND* mysql_bind = stmt->getMYSQL_BIND();

    if (::mysql_stmt_bind_param(mysql_stmt, mysql_bind)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_stmt_error(mysql_stmt);
        FAKE_LOG(ERROR) << "mysql_stmt_bind_param " << err_no << ":" << err_str;
        storeError(err_no, err_str);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return execute(stmt);       // Try again
        return false;
    }

    if (::mysql_stmt_execute(mysql_stmt)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_stmt_error(mysql_stmt);
        FAKE_LOG(ERROR) << "mysql_stmt_execute " << err_no << ":" << err_str;
        storeError(err_no, err_str);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return execute(stmt);       // Try again
        return false;
    }
    return true;
}

bool Connection::queryDetail(PreparedStatement& stmt)
{
    if (!m_mysql) {
        storeError(CR_UNKNOWN_ERROR, nullptr);
        return false;
    }

    MYSQL_STMT* msql_stmt = stmt.getMYSQL_STMT();
    MYSQL_BIND* msql_bind = stmt.getMYSQL_BIND();
    if (::mysql_stmt_bind_param(msql_stmt, msql_bind)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_stmt_error(msql_stmt);
        FAKE_LOG(ERROR) << "mysql_stmt_bind_param " << err_no << ":" << err_str;
        storeError(err_no, err_str);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return queryDetail(stmt);       // Try again
        return false;
    }

    if (::mysql_stmt_execute(msql_stmt)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_stmt_error(msql_stmt);
        FAKE_LOG(ERROR) << "mysql_stmt_execute " << err_no << ":" << err_str; 
        storeError(err_no, err_str);

        if (handleMySQLErrno(err_no))  // If it returns true, an error was handled successfully (i.e. reconnection)
            return queryDetail(stmt);      // Try again
        return false;
    }
    return true;
}

bool Connection::queryDetail(const char *sql)
{
    if (!m_mysql) {
        storeError(CR_UNKNOWN_ERROR, nullptr);
        return false;
    }

    if (::mysql_query(m_mysql, sql)) {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_error(m_mysql);
        FAKE_LOG(ERROR) << "mysql_query " << err_no << ":" << err_str;
        storeError(err_no, err_str);

        if (handleMySQLErrno(err_no))      // If it returns true, an error was handled successfully (i.e. reconnection)
            return queryDetail(sql);    // We try again
        return false;
    }
    return true;
}

void Connection::beginTransaction()
{
    execute("START TRANSACTION");
}

void Connection::rollbackTransaction()
{
    execute("ROLLBACK");
}

void Connection::commitTransaction()
{
    execute("COMMIT");
}

PreparedStatementUPtr Connection::prepareStmt(const char* sql)
{
    MYSQL_STMT* stmt = ::mysql_stmt_init(m_mysql);
    if (!stmt) {
        uint32 err_no = ::mysql_errno(m_mysql);
        const char* err_str = ::mysql_error(m_mysql);
        FAKE_LOG(ERROR) << "mysql_stmt_init " << err_no << ":" << err_str;
        storeError(err_no, err_str);
        return nullptr;
    }

    int ret = ::mysql_stmt_prepare(stmt, sql, static_cast<unsigned long>(std::strlen(sql)));
    if (ret != 0) {
        uint32 err_no = (uint32)ret;
        const char* err_str = ::mysql_stmt_error(stmt);
        FAKE_LOG(ERROR) << "mysql_stmt_prepare " << ret << ":" << err_str;
        storeError(err_no, err_str);
        ::mysql_stmt_close(stmt);
        return nullptr;
    }
    return util::make_unique<PreparedStatement>(stmt);
}

bool Connection::handleMySQLErrno(uint32 err_no)
{
    switch (err_no) {
    case CR_SERVER_GONE_ERROR:
    case CR_SERVER_LOST:
    case CR_INVALID_CONN_HANDLE:
    case CR_SERVER_LOST_EXTENDED: {
        //FAKE_LOG(ERROR) << "handle mysql error: lost the connection to the MySQL server!";
        /*no break*/
    }
    case CR_CONN_HOST_ERROR: {
        if (m_conn_info.auto_reconn) {
            if (m_mysql) {
                ::mysql_close(m_mysql);
                m_mysql = nullptr;
            }
            FAKE_LOG(INFO) << "try to reconnect to the MySQL server...";
            const uint32 err_no_ex = open();
            if (err_no_ex == 0) {
                FAKE_LOG(INFO) << "successfully reconnected to " << m_conn_info.database 
                    << " " << m_conn_info.host << " " << m_conn_info.port;
                return true;
            } else {
                FAKE_LOG(ERROR) << "failed to reconnect to the MySQL server,terminating the server to prevent data corruption!";
                return false;
            }
        }
    }
    default:
        FAKE_LOG(ERROR) <<  "Unhandled MySQL errno:" << err_no << " Unexpected behaviour possible.";
        return false;
    }
}

void Connection::storeError(uint32 err_no, const char* err_str)
{
    m_err_no = err_no;
    m_err_str = err_str ? err_str : &Connection::m_null;
}

void Connection::ping() { ::mysql_ping(m_mysql); }
MYSQL* Connection::getMYSQL() { return m_mysql; }
uint32 Connection::getErrno() const { return m_err_no; }
const char* Connection::getError() const { return m_err_str; }

}
