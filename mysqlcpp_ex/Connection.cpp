#include "Connection.h"

#include <mysql.h>
#include <errmsg.h>

#include "PreparedStatement.h"
#include "Statement.h"
#include "FakeLog.h"
#include "Utils.h"

namespace mysqlcpp {

const char Connection::m_null = '\0';

Connection::Connection(ConnectionOpt conn_opt) 
	: m_mysql(nullptr)
    , m_conn_info(conn_opt)
    , m_err_no(0)
    , m_err_str(&Connection::m_null)
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

void Connection::storeError(uint32 err_no, const char* err_str)
{
    m_err_no = err_no;
    m_err_str = err_str ? err_str : &Connection::m_null;
}

int Connection::ping() { return ::mysql_ping(m_mysql); }
MYSQL* Connection::getMYSQL() { return m_mysql; }
uint32 Connection::getErrno() const { return m_err_no; }
const char* Connection::getError() const { return m_err_str; }

StatementPtr Connection::createStatement()
{
    return std::make_shared<Statement>(*this);
}

PreparedStatementPtr Connection::preparedStatement(const char* sql)
{
    return createPreparedStatement(sql, std::strlen(sql));
}

PreparedStatementPtr Connection::preparedStatement(const std::string& sql)
{
    return createPreparedStatement(sql.c_str(), sql.size());
}

PreparedStatementPtr Connection::createPreparedStatement(const char* sql, size_t sql_len)
{
    MYSQL_STMT* stmt = ::mysql_stmt_init(getMYSQL());
    if (!stmt) {
        uint32 err_no = ::mysql_errno(getMYSQL());
        const char* err_str = ::mysql_error(getMYSQL());
        FAKE_LOG(ERROR) << "mysql_stmt_init " << err_no << ":" << err_str;
        storeError(err_no, err_str);
        return nullptr;
    }

    int ret = ::mysql_stmt_prepare(stmt, sql, static_cast<unsigned long>(sql_len));
    if (ret != 0) {
        uint32 err_no = (uint32)ret;
        const char* err_str = ::mysql_stmt_error(stmt);
        FAKE_LOG(ERROR) << "mysql_stmt_prepare " << ret << ":" << err_str;
        storeError(err_no, err_str);
        ::mysql_stmt_close(stmt);
        return nullptr;
    }
    return std::make_shared<PreparedStatement>(*this, stmt);
}


}
