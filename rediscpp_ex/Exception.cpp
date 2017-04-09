#include "Exception.h"

namespace rediscpp {

Exception::Exception(const std::string& s)
    : std::runtime_error(s)
{
}

Exception::Exception(const char* s)
    : std::runtime_error(s)
{
}


ReplyNullException::ReplyNullException(const std::string& s)
    : Exception(s)
{
}

ReplyNullException::ReplyNullException(const char* s)
    : Exception(s)
{
}


ReplyErrorException::ReplyErrorException(const std::string& s)
    : Exception(s)
{
}

ReplyErrorException::ReplyErrorException(const char* str)
    : Exception(str)
{
}

ReplyTypeException::ReplyTypeException(const char* str)
    : Exception(str)
{
}

ReplyTypeException::ReplyTypeException(const std::string& str)
    : Exception(str)
{
}

}
