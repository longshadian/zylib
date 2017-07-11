#include "Field.h"

#include <cstring>

#include "FakeLog.h"
#include "DateTime.h"
#include "Utils.h"
#include "MyAssert.h"

namespace mysqlcpp {

Field::Field()
    : m_type(MYSQL_TYPE_DECIMAL)
    , m_buffer()
    , m_length()
    , m_is_binary()
    , m_is_null()
{
}

Field::~Field()
{
}

Field::Field(const Field& rhs)
    : m_type(rhs.m_type)
    , m_buffer(rhs.m_buffer)
    , m_length(rhs.m_length)
    , m_is_binary(rhs.m_is_binary)
    , m_is_null(rhs.m_is_null)
{
}

Field& Field::operator=(const Field& rhs)
{
    if (this != &rhs) {
        m_type = rhs.m_type;
        m_buffer = rhs.m_buffer;
        m_length = rhs.m_length;
        m_is_binary = rhs.m_is_binary;
        m_is_null = rhs.m_is_null;
    }
    return *this;
}

Field::Field(Field&& rhs)
    : m_type(std::move(rhs.m_type))
    , m_buffer(std::move(rhs.m_buffer))
    , m_length(std::move(rhs.m_length))
    , m_is_binary(std::move(rhs.m_is_binary))
    , m_is_null(std::move(rhs.m_is_null))
{
}

Field& Field::operator=(Field&& rhs)
{
    if (this != &rhs) {
        m_type = std::move(rhs.m_type);
        m_buffer = std::move(rhs.m_buffer);
        m_length = std::move(rhs.m_length);
        m_is_binary = std::move(rhs.m_is_binary);
        m_is_null = std::move(rhs.m_is_null);
    }
    return *this;
}

void Field::setBinaryValue(enum_field_types type, void* src, unsigned long src_len, bool raw_bytes)
{
    m_type = type;
    m_is_binary = raw_bytes;
    if (src) {
        m_buffer.resize(src_len);
        m_length = src_len;
        std::memcpy(m_buffer.data(), src, src_len);
    } else {
        m_buffer.clear();
        m_length = 0;
    }
}

void Field::setNullValue(enum_field_types type)
{
    m_type = type;
    m_is_null = true;
}

bool Field::getBool() const 
{ 
    return getUInt8() == 1; 
}

uint8 Field::getUInt8() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_TINY)) {
        FAKE_LOG(ERROR) << "Warning: GetUInt8() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint8 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<uint8>(std::strtoul((char*)m_buffer.data(), nullptr, 10));
}

int8 Field::getInt8() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_TINY)) {
        FAKE_LOG(ERROR) << "Warning: GetInt8() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int8 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<int8>(std::strtol((char*)m_buffer.data(), NULL, 10));
}

uint16 Field::getUInt16() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_SHORT) && !isType(MYSQL_TYPE_YEAR)) {
        FAKE_LOG(ERROR) << "Warning: GetUInt16() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint16 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<uint16>(std::strtoul((char*)m_buffer.data(), nullptr, 10));
}

int16 Field::getInt16() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_SHORT) && !isType(MYSQL_TYPE_YEAR)) {
        FAKE_LOG(ERROR) << "Warning: GetInt16() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int16 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<int16>(std::strtol((char*)m_buffer.data(), NULL, 10));
}

uint32 Field::getUInt32() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_INT24) && !isType(MYSQL_TYPE_LONG)) {
        FAKE_LOG(ERROR) << "Warning: GetUInt32() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint32 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<uint32>(std::strtoul((char*)m_buffer.data(), nullptr, 10));
}

int32 Field::getInt32() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_INT24) && !isType(MYSQL_TYPE_LONG)) {
        FAKE_LOG(ERROR) << "Warning: GetInt32() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int32 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<int32>(std::strtol((char*)m_buffer.data(), NULL, 10));
}

uint64 Field::getUInt64() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_LONGLONG) && !isType(MYSQL_TYPE_BIT)) {
        FAKE_LOG(ERROR) << "Warning: GetUInt64() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint64 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<uint64>(std::strtoull((char*)m_buffer.data(), nullptr, 10));
}

int64 Field::getInt64() const
{
    if (isNull())
        return 0;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_LONGLONG) && !isType(MYSQL_TYPE_BIT)) {
        FAKE_LOG(ERROR) << "Warning: GetInt64() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int64 val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<int64>(std::strtoll((char*)m_buffer.data(), NULL, 10));
}

float Field::getFloat() const
{
    if (isNull())
        return 0.0f;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_FLOAT)) {
        FAKE_LOG(ERROR) << "Warning: GetFloat() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        float val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return static_cast<float>(std::strtof((char*)m_buffer.data(), nullptr));
}

double Field::getDouble() const
{
    if (isNull())
        return 0.0f;

    #ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_DOUBLE) && !isType(MYSQL_TYPE_NEWDECIMAL)) {
        FAKE_LOG(ERROR) << "Warning: GetDouble() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        double val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return std::strtod((char*)m_buffer.data(), nullptr);
}

long double Field::getLongDouble() const
{
    if (isNull())
        return 0.0f;
#ifdef TRINITY_DEBUG
    if (!isType(MYSQL_TYPE_DOUBLE) && !isType(MYSQL_TYPE_NEWDECIMAL)) {
        FAKE_LOG(ERROR) << "Warning: GetDouble() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
#endif

    if (m_is_binary) {
        long double val = 0;
        std::memcpy(&val, m_buffer.data(), sizeof(val));
        return val;
    }
    return std::strtold((char*)m_buffer.data(), nullptr);
}

/*
char const* Field::getCString() const
{
    if (!data.value)
        return NULL;

    #ifdef TRINITY_DEBUG
    if (isNumeric())
    {
        FAKE_LOG(ERROR) << "Warning: GetCString() on non-tinyint field " << meta.TableAlias
            << " " << meta.Alias
            << " " << meta.TableName
            << " " << meta.Name
            << " " << meta.Index
            << " " << meta.Type;
        return NULL;
    }
    #endif
    return static_cast<char const*>(data.value);
}
*/

std::string Field::getString() const
{
    if (isNull())
        return "";
    const char* p = (const char*)m_buffer.data();
    return std::string(p, p + m_length);
}

std::vector<uint8> Field::getBinary() const
{
    if (isNull())
        return {};
    return std::vector<uint8>(m_buffer.data(), m_buffer.data() + m_length);
}

bool Field::isNull() const
{
    return m_is_null;
}

DateTime Field::getDateTime() const
{
    if (isNull())
        return DateTime{};
    if (m_is_binary){
        MYSQL_TIME mysql_time{};
        std::memset(&mysql_time, 0, sizeof(mysql_time));
        std::memcpy(&mysql_time, m_buffer.data(), m_length);
        return DateTime(mysql_time);
    }
    DateTime tm{};
    MY_ASSERT(util::datetimeFromString(&tm, getString()));
    return tm;
}

}
