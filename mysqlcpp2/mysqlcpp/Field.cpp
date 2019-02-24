#include "mysqlcpp/Field.h"

#include <cstring>
#include <string_view>

#include "mysqlcpp/MysqlcppLog.h"
#include "mysqlcpp/DateTime.h"
#include "mysqlcpp/Utility.h"
#include "mysqlcpp/MysqlcppAssert.h"

#include "mysqlcpp/detail/Convert.h"

namespace mysqlcpp {

static const char* fieldTypeName(enum_field_types type)
{
    switch (type)
    {
        case MYSQL_TYPE_BIT:         return "BIT";
        case MYSQL_TYPE_BLOB:        return "BLOB";
        case MYSQL_TYPE_DATE:        return "DATE";
        case MYSQL_TYPE_DATETIME:    return "DATETIME";
        case MYSQL_TYPE_NEWDECIMAL:  return "NEWDECIMAL";
        case MYSQL_TYPE_DECIMAL:     return "DECIMAL";
        case MYSQL_TYPE_DOUBLE:      return "DOUBLE";
        case MYSQL_TYPE_ENUM:        return "ENUM";
        case MYSQL_TYPE_FLOAT:       return "FLOAT";
        case MYSQL_TYPE_GEOMETRY:    return "GEOMETRY";
        case MYSQL_TYPE_INT24:       return "INT24";
        case MYSQL_TYPE_LONG:        return "LONG";
        case MYSQL_TYPE_LONGLONG:    return "LONGLONG";
        case MYSQL_TYPE_LONG_BLOB:   return "LONG_BLOB";
        case MYSQL_TYPE_MEDIUM_BLOB: return "MEDIUM_BLOB";
        case MYSQL_TYPE_NEWDATE:     return "NEWDATE";
        case MYSQL_TYPE_NULL:        return "NULL";
        case MYSQL_TYPE_SET:         return "SET";
        case MYSQL_TYPE_SHORT:       return "SHORT";
        case MYSQL_TYPE_STRING:      return "STRING";
        case MYSQL_TYPE_TIME:        return "TIME";
        case MYSQL_TYPE_TIMESTAMP:   return "TIMESTAMP";
        case MYSQL_TYPE_TINY:        return "TINY";
        case MYSQL_TYPE_TINY_BLOB:   return "TINY_BLOB";
        case MYSQL_TYPE_VAR_STRING:  return "VAR_STRING";
        case MYSQL_TYPE_YEAR:        return "YEAR";
        default:                     return "-Unknown-";
    }
}

FieldMeta::FieldMeta(const MYSQL_FIELD* field, uint32 field_index)
    : m_table_name()
    , m_table_alias()
    , m_name()
    , m_alias()
    , m_type_name()
    , m_type(field->type)
    , m_index(field_index)
{
    if (field->org_table)
        m_table_name = field->org_table;
    if (field->table)
        m_table_alias = field->table;
    if (field->org_name)
        m_name = field->org_name;
    if (field->name)
        m_alias = field->name;
    m_type_name = fieldTypeName(m_type);

    /*
    FAKE_LOG(INFO) << "table_name:" << m_meta.m_table_name 
        << " " << "table_alias:" << m_meta.m_table_alias 
        << " " << "name:" << m_meta.m_name 
        << " " << "alias:" << m_meta.m_alias
        << " " << "type:" << m_meta.m_type 
        << " " << "index:" << m_meta.m_index;
        */
}

FieldMeta::FieldMeta(const FieldMeta& rhs)
    : m_table_name(rhs.m_table_name)
    , m_table_alias(rhs.m_table_alias)
    , m_name(rhs.m_name)
    , m_alias(rhs.m_alias)
    , m_type_name(rhs.m_type_name)
    , m_type(rhs.m_type)
    , m_index(rhs.m_index)
{
}

FieldMeta& FieldMeta::operator=(const FieldMeta& rhs)
{
    if (this != &rhs) {
        m_table_name = rhs.m_table_name;
        m_table_alias = rhs.m_table_alias;
        m_name = rhs.m_name;
        m_alias = rhs.m_alias;
        m_type_name = rhs.m_type_name;
        m_type = rhs.m_type;
        m_index = rhs.m_index;
    }
    return *this;
}

FieldMeta::FieldMeta(FieldMeta&& rhs)
    : m_table_name(std::move(rhs.m_table_name))
    , m_table_alias(std::move(rhs.m_table_alias))
    , m_name(std::move(rhs.m_name))
    , m_alias(std::move(rhs.m_alias))
    , m_type_name(std::move(rhs.m_type_name))
    , m_type(std::move(rhs.m_type))
    , m_index(std::move(rhs.m_index))
{
}

FieldMeta& FieldMeta::operator=(FieldMeta&& rhs)
{
    if (this != &rhs) {
        m_table_name    = std::move(rhs.m_table_name);
        m_table_alias   = std::move(rhs.m_table_alias);
        m_name          = std::move(rhs.m_name);
        m_alias         = std::move(rhs.m_alias);
        m_type_name     = std::move(rhs.m_type_name);
        m_type          = std::move(rhs.m_type);
        m_index         = std::move(rhs.m_index);
    }
    return *this;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Field::Field()
    : m_type(MYSQL_TYPE_DECIMAL)
    , m_buffer()
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
    , m_is_binary(rhs.m_is_binary)
    , m_is_null(rhs.m_is_null)
{
}

Field& Field::operator=(const Field& rhs)
{
    if (this != &rhs) {
        m_type = rhs.m_type;
        m_buffer = rhs.m_buffer;
        m_is_binary = rhs.m_is_binary;
        m_is_null = rhs.m_is_null;
    }
    return *this;
}

Field::Field(Field&& rhs)
    : m_type(std::move(rhs.m_type))
    , m_buffer(std::move(rhs.m_buffer))
    , m_is_binary(std::move(rhs.m_is_binary))
    , m_is_null(std::move(rhs.m_is_null))
{
}

Field& Field::operator=(Field&& rhs)
{
    if (this != &rhs) {
        m_type = std::move(rhs.m_type);
        m_buffer = std::move(rhs.m_buffer);
        m_is_binary = std::move(rhs.m_is_binary);
        m_is_null = std::move(rhs.m_is_null);
    }
    return *this;
}

void Field::SetBinaryValue(enum_field_types type, void* src, unsigned long src_len, bool raw_bytes)
{
    m_type = type;
    m_is_binary = raw_bytes;
    m_buffer.Clear();
    if (src) {
        m_buffer.AppendBuffer(src, src_len);
    }
}

void Field::SetNullValue(enum_field_types type)
{
    m_type = type;
    m_is_null = true;
}

bool Field::AsBool() const 
{ 
    return AsUInt8() == 1; 
}

uint8 Field::AsUInt8() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_TINY)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetUInt8() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint8 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<uint8>::cvt_noexcept(m_buffer.AsStringView());
}

int8 Field::AsInt8() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_TINY)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetInt8() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int8 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<int8>::cvt_noexcept(m_buffer.AsStringView());
}

uint16 Field::AsUInt16() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_SHORT) && !isType(MYSQL_TYPE_YEAR)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetUInt16() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint16 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<uint16>::cvt_noexcept(m_buffer.AsStringView());
}

int16 Field::AsInt16() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_SHORT) && !isType(MYSQL_TYPE_YEAR)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetInt16() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int16 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<int16>::cvt_noexcept(m_buffer.AsStringView());
}

uint32 Field::AsUInt32() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_INT24) && !isType(MYSQL_TYPE_LONG)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetUInt32() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint32 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<uint32>::cvt_noexcept(m_buffer.AsStringView());
}

int32 Field::AsInt32() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_INT24) && !isType(MYSQL_TYPE_LONG)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetInt32() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int32 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<int32>::cvt_noexcept(m_buffer.AsStringView());
}

uint64 Field::AsUInt64() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_LONGLONG) && !isType(MYSQL_TYPE_BIT)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetUInt64() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        uint64 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<uint64>::cvt_noexcept(m_buffer.AsStringView());
}

int64 Field::AsInt64() const
{
    if (IsNull())
        return 0;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_LONGLONG) && !isType(MYSQL_TYPE_BIT)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetInt64() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        int64 val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<int64>::cvt_noexcept(m_buffer.AsStringView());
}

float Field::AsFloat() const
{
    if (IsNull())
        return 0.0f;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_FLOAT)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetFloat() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        float val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<float>::cvt_noexcept(m_buffer.AsStringView());
}

double Field::AsDouble() const
{
    if (IsNull())
        return 0.0f;

    #ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_DOUBLE) && !isType(MYSQL_TYPE_NEWDECIMAL)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetDouble() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
    #endif

    if (m_is_binary) {
        double val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<double>::cvt_noexcept(m_buffer.AsStringView());
}

long double Field::AsLongDouble() const
{
    if (IsNull())
        return 0.0f;
#ifdef MYSQLCPP_DEBUG
    if (!isType(MYSQL_TYPE_DOUBLE) && !isType(MYSQL_TYPE_NEWDECIMAL)) {
        MYSQLCPP_LOG(ERROR) << "Warning: GetDouble() on non-tinyint field " << m_meta.m_table_alias << " " << m_meta.m_alias << " " << m_meta.m_table_name << " " << m_meta.m_name << " " << m_meta.m_index << " " << m_meta.m_type;
        return 0;
    }
#endif

    if (m_is_binary) {
        long double val = 0;
        std::memcpy(&val, m_buffer.Data(), sizeof(val));
        return val;
    }
    return detail::Convert<long double>::cvt_noexcept(m_buffer.AsStringView());
}

std::string_view Field::AsStringView() const
{
    if (IsNull())
        return std::string_view{};
    return m_buffer.AsStringView();
}

std::string Field::AsString() const
{
    if (IsNull())
        return "";
    return std::string(m_buffer.AsStringView());
}

std::vector<uint8> Field::AsBinary() const
{
    if (IsNull())
        return {};
    return m_buffer.AsBinary();
}

bool Field::IsNull() const
{
    return m_is_null;
}

DateTime Field::AsDateTime() const
{
    if (IsNull())
        return DateTime{};

    MYSQL_TIME mysql_time{};
    utility::bzero(&mysql_time);
    if (m_is_binary){
        std::memcpy(&mysql_time, m_buffer.Data(), m_buffer.Length());
        return DateTime(mysql_time);
    }
    if (m_type == MYSQL_TYPE_DATE) {
        utility::stringTo_Date(AsString(), &mysql_time.year, &mysql_time.month, &mysql_time.day);
    }
    if (m_type == MYSQL_TYPE_DATETIME || m_type == MYSQL_TYPE_TIMESTAMP) {
        utility::stringTo_DateTime_Timestamp(AsString()
            , &mysql_time.year, &mysql_time.month, &mysql_time.day
            , &mysql_time.hour, &mysql_time.minute, &mysql_time.second
            , &mysql_time.second_part);
    }
    return DateTime{mysql_time};
}

} // mysqlcpp

