#ifndef _MYSQLCPP_STATEMENT_H
#define _MYSQLCPP_STATEMENT_H

#include <mysql.h>
#include <vector>

#include "Types.h"
#include "DateTime.h"

namespace mysqlcpp {

class SQLString;
class FieldMeta;
class Connection;
class Field;

class Statement
{
public:
    Statement(Connection& conn);
    ~Statement();
    Statement(const Statement& right) = delete;
    Statement& operator=(const Statement& right) = delete;
public:
    Connection* getConnection();

    bool            execute(const std::string& sql);
    ResultSetPtr    executeQuery(const std::string& sql);

private:
    bool        checkConnection();
    ResultSetPtr createResultSet();
    bool storeResult(MYSQL_RES* mysql_res, std::vector<RowData>* rows, const FieldData* fields_data);

private:
    Connection& m_conn;
};

}

#endif
