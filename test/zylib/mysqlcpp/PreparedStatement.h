#ifndef _MYSQLCPP_PREPAREDSTATEMENT_H
#define _MYSQLCPP_PREPAREDSTATEMENT_H

#include <mysql.h>
#include <vector>

#include "Types.h"

namespace mysqlcpp {

//- Union for data buffer (upper-level bind -> queue -> lower-level bind)
union PreparedStatementDataUnion
{
    bool boolean;
    uint8 ui8;
    int8 i8;
    uint16 ui16;
    int16 i16;
    uint32 ui32;
    int32 i32;
    uint64 ui64;
    int64 i64;
    float f;
    double d;
};

//- This enum helps us differ data held in above union
enum PreparedStatementValueType
{
    TYPE_BOOL,
    TYPE_UI8,
    TYPE_UI16,
    TYPE_UI32,
    TYPE_UI64,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_BINARY,
    TYPE_NULL
};

struct PreparedStatementData
{
    PreparedStatementDataUnion data;
    PreparedStatementValueType type;
    std::vector<uint8> binary;
};

class MySQLPreparedStatement
{
    friend class MySQLConnection;
public:
    MySQLPreparedStatement(MYSQL_STMT* stmt);
    ~MySQLPreparedStatement();

    void setBool(const uint8 index, const bool value);
    void setUInt8(const uint8 index, const uint8 value);
    void setUInt16(const uint8 index, const uint16 value);
    void setUInt32(const uint8 index, const uint32 value);
    void setUInt64(const uint8 index, const uint64 value);
    void setInt8(const uint8 index, const int8 value);
    void setInt16(const uint8 index, const int16 value);
    void setInt32(const uint8 index, const int32 value);
    void setInt64(const uint8 index, const int64 value);
    void setFloat(const uint8 index, const float value);
    void setDouble(const uint8 index, const double value);
    void setBinary(const uint8 index, const std::vector<uint8>& value, bool isString);
    void setNull(const uint8 index);

    void clearParameters();
    MYSQL_STMT* getMYSQL_STMT();
    MYSQL_BIND* getMYSQL_BIND();
private:
    bool checkValidIndex(uint8 index);
    //std::string getQueryString(std::string const& sqlPattern) const;
    void setWholeNumber(const uint8 index, MYSQL_BIND* param, enum_field_types type, const void* src, uint32 src_len, bool is_unsigned);
    void setRealNumber(const uint8 index, MYSQL_BIND* param, enum_field_types type, const void* src, uint32 src_len);
private:
    MYSQL_STMT*             m_stmt;
    std::vector<MYSQL_BIND> m_bind_param;
    std::vector<bool>       m_param_set;
    uint32_t                m_param_count;
    std::vector<std::vector<char>> m_bind_param_buffer;

    MySQLPreparedStatement(MySQLPreparedStatement const& right) = delete;
    MySQLPreparedStatement& operator=(MySQLPreparedStatement const& right) = delete;
};

}

#endif
