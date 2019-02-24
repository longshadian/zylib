#pragma once

#include <cstring>
#include <vector>
#include <string_view>
#include <string>

#include <mysql.h>

#include "mysqlcpp/Types.h"
#include "mysqlcpp/detail/SafeString.h"

namespace mysqlcpp {

class DateTime;

class MYSQLCPP_EXPORT FieldMeta
{
public:
    FieldMeta(const MYSQL_FIELD* field, uint32 field_index);
    ~FieldMeta() = default;

    FieldMeta(const FieldMeta& rhs);
    FieldMeta& operator=(const FieldMeta& rhs);

    FieldMeta(FieldMeta&& rhs);
    FieldMeta& operator=(FieldMeta&& rhs);

    std::string m_table_name;
    std::string m_table_alias;
    std::string m_name;
    std::string m_alias;
    std::string m_type_name;
    enum_field_types m_type;
    uint32      m_index;
};


/**
	MYSQL查询结果对应类型
    |------------------------|----------------------------|
    | TINYINT                | getBool, getInt8, getUInt8 |
    | SMALLINT               | getInt16, getUInt16        |
    | MEDIUMINT, INT         | getInt32, getUInt32        |
    | BIGINT                 | getInt64, getUInt64        |
    | FLOAT                  | getFloat                   |
    | DOUBLE, DECIMAL        | getDouble                  |
    | CHAR, VARCHAR,         | getString                  |
    | TINYTEXT, MEDIUMTEXT,  | getString                  |
    | TEXT, LONGTEXT         | getString                  |
    | TINYBLOB, MEDIUMBLOB,  | getBinary, getString       |
    | BLOB, LONGBLOB         | getBinary, getString       |
    | BINARY, VARBINARY      | getBinary                  |
    | DATE, TIME			 | getDateTime()			  |
	| DATETIME, TIMESTAMP    | getDateTime()              |

	聚合函数返回值：
    |----------|------------|
    | MIN, MAX | 和Field类似 |
    | SUM, AVG | getDouble  |
    | COUNT    | getInt64   |
*/
class MYSQLCPP_EXPORT Field
{
    friend class PreparedStatement;
    friend class Statement;
public:
    Field();
    ~Field();
    Field(const Field& rhs);
    Field& operator=(const Field& rhs);
    Field(Field&& rhs);
    Field& operator=(Field&& rhs);
public:

    bool                AsBool() const;
    uint8               AsUInt8() const;
    int8                AsInt8() const;
    uint16              AsUInt16() const;
    int16               AsInt16() const;
    uint32              AsUInt32() const;
    int32               AsInt32() const;
    uint64              AsUInt64() const;
    int64               AsInt64() const;
    float               AsFloat() const;
    double              AsDouble() const;
    long double         AsLongDouble() const;
    std::string_view    AsStringView() const;
    std::string         AsString() const;
    std::vector<uint8>  AsBinary() const;
    bool                IsNull() const;
    DateTime            AsDateTime() const;
    const detail::SafeString& GetInternalBuffer() const { return m_buffer; }

private:
    void                SetBinaryValue(enum_field_types type, void* src, unsigned long src_len, bool raw_bytes);
    void                SetNullValue(enum_field_types type);

private:
    enum_field_types    m_type;
    detail::SafeString  m_buffer;
    bool                m_is_binary;
    bool                m_is_null;
};

}
