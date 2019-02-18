#include "QueryResult.h"

namespace mysqlcpp {

ResultSet::ResultSet(MYSQL_RES *result, MYSQL_FIELD *fields, uint64 rowCount, uint32 fieldCount) :
    m_row_count(rowCount),
    m_field_count(fieldCount),
    m_result(result),
    m_fields(fields)
{
    m_current_row.resize(m_field_count);
#ifdef TRINITY_DEBUG
    for (uint32 i = 0; i < m_field_count; i++)
        m_current_row[i].SetMetadata(&m_fields[i], i);
#endif
}

ResultSet::~ResultSet()
{
    CleanUp();
}

bool ResultSet::NextRow()
{
    MYSQL_ROW row;

    if (!m_result)
        return false;

    row = mysql_fetch_row(m_result);
    if (!row)
    {
        CleanUp();
        return false;
    }

    unsigned long* lengths = mysql_fetch_lengths(m_result);
    if (!lengths)
    {
        FAKE_LOG_ERROR() << "mysql_fetch_lengths, cannot retrieve value lengths. Error " << ::mysql_error(m_result->handle);
        CleanUp();
        return false;
    }

    for (uint32 i = 0; i < m_field_count; i++)
        m_current_row[i].SetStructuredValue(row[i], m_fields[i].type, lengths[i]);

    return true;
}

void ResultSet::CleanUp()
{
    m_current_row.clear();
    if (m_result) {
        ::mysql_free_result(m_result);
        m_result = NULL;
    }
}


PreparedResultSet::PreparedResultSet(MYSQL_STMT* stmt, MYSQL_RES *result, uint64 row_count, uint32 field_count) 
    : m_row_count(row_count)
    , m_row_position(0)
    , m_field_count(field_count)
    , m_stmt(stmt)
    , m_metadata_result(result)
    , m_out_bind()
    , m_out_is_null()
    , m_out_length()
    , m_out_row()
{
    if (!m_metadata_result)
        return;

    m_field_count = ::mysql_num_fields(result);

    if (::mysql_stmt_store_result(m_stmt)) {
        FAKE_LOG_ERROR() << "mysql_stmt_store_result, cannot bind result from MySQL server. Error:" << ::mysql_stmt_error(m_stmt);
        return;
    }

    m_out_bind.resize(m_field_count);
    m_out_is_null.resize(m_field_count);
    m_out_length.resize(m_field_count);
    std::memset(m_out_bind.data(), 0, sizeof(MYSQL_BIND) * m_out_bind.size());
    std::memset(m_out_is_null.data(), 0, sizeof(my_bool) * m_out_is_null.size());
    std::memset(m_out_length.data(), 0, sizeof(unsigned long) * m_out_length.size());

    m_row_count = ::mysql_stmt_num_rows(m_stmt);

    // ×¼±¸buffer
    std::vector<std::vector<char>> out_bind_buffer{};
    MYSQL_FIELD* field = ::mysql_fetch_fields(m_metadata_result);
    out_bind_buffer.resize(m_field_count);
    for (uint32_t i = 0; i != m_field_count; ++i) {
        uint32 size = Field::SizeForType(&field[i]);

        std::vector<char> buffer{};
        buffer.resize(size);
        out_bind_buffer[i] = std::move(buffer);

        m_out_bind[i].buffer = out_bind_buffer[i].data();
        m_out_bind[i].buffer_type = field[i].type;
        m_out_bind[i].length = &m_out_length[i];
        m_out_bind[i].is_null = &m_out_is_null[i];
        m_out_bind[i].error = nullptr;
        m_out_bind[i].is_unsigned = field[i].flags & UNSIGNED_FLAG;
    }

    //- This is where we bind the bind the buffer to the statement
    if (mysql_stmt_bind_result(m_stmt, m_out_bind.data())) {
        FAKE_LOG_ERROR() << "mysql_stmt_bind_result, cannot bind result from MySQL server. Error: " << ::mysql_stmt_error(m_stmt);
        ::mysql_stmt_free_result(m_stmt);
        cleanUp();
        return;
    }

    m_out_row.resize(m_row_count * m_field_count);
    while (nextRowDetail()) {
        for (uint32 i = 0; i < m_field_count; ++i) {
            //unsigned long buffer_length = m_out_bind[i].buffer_length;
            unsigned long fetched_length = *m_out_bind[i].length;
            if (!*m_out_bind[i].is_null) {
                void* buffer = m_stmt->bind[i].buffer;
                /*
                switch (m_rBind[i].buffer_type) {
                case MYSQL_TYPE_TINY_BLOB:
                case MYSQL_TYPE_MEDIUM_BLOB:
                case MYSQL_TYPE_LONG_BLOB:
                case MYSQL_TYPE_BLOB:
                case MYSQL_TYPE_STRING:
                case MYSQL_TYPE_VAR_STRING:
                        // warning - the string will not be null-terminated if there is no space for it in the buffer
                        // when mysql_stmt_fetch returned MYSQL_DATA_TRUNCATED
                        // we cannot blindly null-terminate the data either as it may be retrieved as binary blob and not specifically a string
                        // in this case using Field::GetCString will result in garbage
                        // TODO: remove Field::GetCString and use boost::string_ref (currently proposed for TS as string_view, maybe in C++17)
                    if (fetched_length < buffer_length)
                        *((char*)buffer + fetched_length) = '\0';
                    break;
                default:
                    break;
                }
                */
                m_out_row[uint32(m_row_position) * m_field_count + i].SetByteValue(buffer, m_out_bind[i].buffer_type, fetched_length);

                // move buffer pointer to next part
                //m_stmt->bind[i].buffer = (char*)buffer + rowSize;
            } else {
                m_out_row[uint32(m_row_position) * m_field_count + i].SetByteValue( nullptr, m_out_bind[i].buffer_type, *m_out_bind[i].length);
            }
        }
        m_row_position++;
    }

    m_row_position = 0;

    /// All data is buffered, let go of mysql c api structures
    mysql_stmt_free_result(m_stmt);
}

PreparedResultSet::~PreparedResultSet()
{
    cleanUp();
}

bool PreparedResultSet::nextRow()
{
    /// Only updates the m_rowPosition so upper level code knows in which element
    /// of the rows vector to look
    if (++m_row_position >= m_row_count)
        return false;
    return true;
}

bool PreparedResultSet::nextRowDetail()
{
    /// Only called in low-level code, namely the constructor
    /// Will iterate over every row of data and buffer it
    if (m_row_position >= m_row_count)
        return false;

    int retval = mysql_stmt_fetch(m_stmt);
    return retval == 0 || retval == MYSQL_DATA_TRUNCATED;
}

void PreparedResultSet::cleanUp()
{
    if (m_metadata_result)
        ::mysql_free_result(m_metadata_result);
}

}
