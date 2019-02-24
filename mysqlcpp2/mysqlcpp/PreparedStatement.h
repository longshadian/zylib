#pragma once

#include <mysql.h>
#include <vector>

#include "mysqlcpp/Types.h"
#include "mysqlcpp/detail/ParamBind.h"
#include "mysqlcpp/detail/ResultBind.h"

namespace mysqlcpp {

class DateTime;
class Connection;

class MYSQLCPP_EXPORT PreparedStatement
{
    enum class NEXT_ROW : int32_t
    {
        FAILED = 0,     // 出错
        SUCCESS = 1,    // 有数据
        NO_DATA_ = 2,    // 没数据
        TRUNCATED = 3,  // 数据被截断
    };

public:
    PreparedStatement(Connection& conn, MYSQL_STMT* stmt);
    ~PreparedStatement();
    PreparedStatement(const PreparedStatement& right) = delete;
    PreparedStatement& operator=(const PreparedStatement& right) = delete;

    Connection* getConnection();
    bool            execute();
    ResultSetPtr    executeQuery();

    void SetBool(uint32 index, bool value);
    void SetUInt8(uint32 index, uint8 value);
    void SetUInt16(uint32 index, uint16 value);
    void SetUInt32(uint32 index, uint32 value);
    void SetUInt64(uint32 index, uint64 value);
    void SetInt8(uint32 index, int8 value);
    void SetInt16(uint32 index, int16 value);
    void SetInt32(uint32 index, int32 value);
    void SetInt64(uint32 index, int64 value);
    void SetFloat(uint32 index, float value);
    void SetDouble(uint32 index, double value);
    void SetString(uint32 index, const std::string& value);
    void SetString(uint32 index, const char* value);
    void SetBinary(uint32 index, std::vector<uint8> value, bool isString);
    void SetNull(uint32 index);
    void SetDateTime(uint32 index, const DateTime& value);

    void ClearParameters();
    unsigned long long AffectedRows();

    MYSQL_STMT* RawMysqlStmt();
    MYSQL_RES* RawtMysqlRes();
private:
    bool bindParam();
    bool bindResult(std::vector<RowData>* all_rows = nullptr, size_t num_field = 0);

    NEXT_ROW nextRow();
private:
    Connection&                         m_conn;
    MYSQL_STMT*                         m_stmt;
    MYSQL_RES*                          m_mysql_res;
    unsigned long                       m_param_count;
    std::shared_ptr<detail::ParamBind>  m_param_bind;
    std::shared_ptr<detail::ResultBind> m_result_bind;
};

}
