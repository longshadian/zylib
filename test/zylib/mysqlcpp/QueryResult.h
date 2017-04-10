#ifndef _MYSQLCPP_QUERYRESULT_H
#define _MYSQLCPP_QUERYRESULT_H

#include <memory>
#include <mysql.h>

#include "Field.h"
#include "Assert.h"

namespace mysqlcpp {

class ResultSet
{
public:
    ResultSet(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount);
    ~ResultSet();

    bool NextRow();
    uint64 GetRowCount() const { return m_row_count; }
    uint32 GetFieldCount() const { return m_field_count; }

    const Field* Fetch() const { return m_current_row.data(); }
    const Field & operator [] (uint32 index) const
    {
        ASSERT(index < m_field_count);
        return m_current_row[index];
    }

private:
    void CleanUp();
private:
    uint64 m_row_count;
    std::vector<Field> m_current_row;
    uint32 m_field_count;
    MYSQL_RES* m_result;
    MYSQL_FIELD* m_fields;

    ResultSet(ResultSet const& right) = delete;
    ResultSet& operator=(ResultSet const& right) = delete;
};


class PreparedResultSet
{
public:
    PreparedResultSet(MYSQL_STMT* stmt, MYSQL_RES* result, uint64 rowCount, uint32 fieldCount);
    ~PreparedResultSet();

    bool nextRow();
    uint64 GetRowCount() const { return m_row_count; }
    uint32 GetFieldCount() const { return m_field_count; }

    Field* Fetch() const
    {
        ASSERT(m_row_position < m_row_count);
        return const_cast<Field*>(&m_out_row[uint32(m_row_position) * m_field_count]);
    }

    Field const& operator[](uint32 index) const
    {
        ASSERT(m_row_position < m_row_count);
        ASSERT(index < m_field_count);
        return m_out_row[uint32(m_row_position) * m_field_count + index];
    }
private:
    void cleanUp();
    bool nextRowDetail();
private:
    uint64 m_row_count;
    uint64 m_row_position;
    uint32 m_field_count;

    MYSQL_STMT*                 m_stmt;
    MYSQL_RES*                  m_metadata_result;    ///< Field metadata, returned by mysql_stmt_result_metadata

    std::vector<MYSQL_BIND>     m_out_bind;
    std::vector<my_bool>        m_out_is_null;
    std::vector<unsigned long>  m_out_length;
    std::vector<Field>          m_out_row;

    PreparedResultSet(PreparedResultSet const& right) = delete;
    PreparedResultSet& operator=(PreparedResultSet const& right) = delete;
};

}

#endif

