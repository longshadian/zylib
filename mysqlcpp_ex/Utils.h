#ifndef _MYSQLCPP_UTILS_H
#define _MYSQLCPP_UTILS_H

#include <mysql.h>

#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include "Types.h"

namespace mysqlcpp {

class DateTime;

namespace util {

class Tokenizer
{
public:
    typedef std::vector<char const*> StorageType;
    typedef StorageType::size_type size_type;
    typedef StorageType::const_iterator const_iterator;
    typedef StorageType::reference reference;
    typedef StorageType::const_reference const_reference;
public:
    Tokenizer(const std::string &src, char sep);
    ~Tokenizer() = default;

    const_iterator begin() const { return m_storage.begin(); }
    const_iterator end() const { return m_storage.end(); }

    bool empty() const { return m_storage.empty(); }
    size_type size() const { return m_storage.size(); }
    reference operator [] (size_type i) { return m_storage[i]; }
    const_reference operator [] (size_type i) const { return m_storage[i]; }
private:
    std::vector<char> m_str;
    StorageType m_storage;
};

template <typename T>
struct Convert
{
    static T convert(const std::vector<uint8>& binary)
    {
        T val{};
        if (binary.empty())
            return val;
        std::memcpy(&val, binary.data(), sizeof(val));
        return val;
    }
};

template <>
struct Convert<std::string>
{
    static std::string convert(const std::vector<uint8>& binary)
    {
        if (binary.empty())
            return{};
        const char* p = (const char*)binary.data();
        return std::string{ p, p + binary.size() };
    }
};

/*
bool datetimeFromStringEx(DateTime* date_time, enum_field_types, const std::string& str);
bool stringToDatetime(const std::string& str, DateTime* date_time);
bool stringToDate(const std::string& str, DateTime* date_time);
bool stringToTime(const std::string& str, DateTime* date_time);
bool stringToYear4(const std::string& str, DateTime* date_time);
*/

bool datetimeFromString(DateTime* date_time, const std::string& str);
bool timeFromString(DateTime* date_time, const std::string& str);
bool dateFromString(DateTime* date_time, const std::string& str);

void bindFiledsMeta(MYSQL_RES* mysql_res, std::vector<FieldMeta>* fields_data);

}
}

#endif
