#pragma once

#include <mysql.h>
#include <vector>

#include "mysqlcpp/Types.h"
#include "mysqlcpp/DateTime.h"

namespace mysqlcpp {

class SQLString;
class FieldMeta;
class Connection;
class Field;

class MYSQLCPP_EXPORT Statement
{
public:
    Statement(Connection& conn);
    ~Statement();
    Statement(const Statement& right) = delete;
    Statement& operator=(const Statement& right) = delete;
public:
    Connection* GetConnection();

    bool            Execute(const std::string& sql);
    ResultSetPtr    ExecuteQuery(const std::string& sql);

private:
    bool            CheckConnection();
    ResultSetPtr    CreateResultSet();
    bool            StoreResult(MYSQL_RES* mysql_res, std::vector<RowData>* rows, const FieldData* fields_data);

private:
    Connection& m_conn;
};

}
