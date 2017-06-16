#ifndef _MYSQLCPP_FIELD_H
#define _MYSQLCPP_FIELD_H

#include <mysql.h>
#include <cstring>

#include <vector>

#include "Types.h"

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

namespace mysqlcpp {

class DateTime;

class Field
{
    struct Slot
    {
        Slot();
        ~Slot();
        Slot(const Slot& rhs);
        Slot& operator=(const Slot& rhs);
        Slot(Slot&& rhs);
        Slot& operator=(Slot&& rhs);

        enum_field_types    m_type;      // Field type
        bool                m_raw;       // Raw bytes? (Prepared statement or ad hoc)
        std::vector<uint8>  m_buffer;
        uint64              m_length;    // Length (prepared strings only)
    };
public:
    Field();
    ~Field();

    Field(const Field& rhs);
    Field& operator=(const Field& rhs);

    Field(Field&& rhs);
    Field& operator=(Field&& rhs);
public:
    static uint32 sizeForType(MYSQL_FIELD* field);

    bool getBool() const;
    uint8 getUInt8() const;
    int8 getInt8() const;
    uint16 getUInt16() const;
    int16 getInt16() const;
    uint32 getUInt32() const;
    int32 getInt32() const;
    uint64 getUInt64() const;
    int64 getInt64() const;
    float getFloat() const;
    double getDouble() const;
    long double getLongDouble() const;
    //char const* getCString() const;
    std::string getString() const;
    std::vector<uint8> getBinary() const;
    bool isNull() const;
    DateTime getDateTime() const;

    void setByteValue(enum_field_types type, void* src, uint64 src_len, bool raw_bytes);

    bool isType(enum_field_types type) const;
    bool isNumeric() const;

    static char const* fieldTypeToString(enum_field_types type);
private:
    Slot        m_data;
};

}

#endif

