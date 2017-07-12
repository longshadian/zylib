#ifndef _MYSQLCPP_RESULTBIND_H
#define _MYSQLCPP_RESULTBIND_H

#include <mysql.h>

#include <vector>
#include <memory>

namespace mysqlcpp {

class PreparedStatement;

struct ResultBindBuffer
{
    ResultBindBuffer(size_t s, enum_field_types t) 
        : m_size(s)
        , m_type(t) 
        , m_buffer(nullptr)
        , buffer_()
    {
        if (s > 0) {
            buffer_.resize(s);
            m_buffer = buffer_.data();
        }
    }

    ~ResultBindBuffer()
    {

    }

    size_t              m_size;
    enum_field_types    m_type;
    char*               m_buffer;
private:
    std::vector<char>   buffer_;
};

class ResultBind
{
public:
	ResultBind();
	~ResultBind();

    ResultBind(const ResultBind& rhs) = delete;
    ResultBind& operator=(const ResultBind& rhs) = delete;

	bool bindResult(PreparedStatement& ps);
    const MYSQL_BIND* getMYSQL_BIND(size_t i) const;
    void clear();
private:
    std::vector<MYSQL_BIND>     m_bind;
    std::vector<my_bool>        m_is_null;
    std::vector<my_bool>        m_err;
    std::vector<unsigned long>  m_len;
    std::vector<std::shared_ptr<ResultBindBuffer>> m_bind_buffer;
};

} // mysqlcpp

#endif
