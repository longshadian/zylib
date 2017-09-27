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

ReplyException::ReplyException(const std::string& s)
    : Exception(s)
{
}

ReplyException::ReplyException(const char* s)
    : Exception(s)
{
}

ConnectionException::ConnectionException(const std::string& s)
    : Exception(s)
{
}

ConnectionException::ConnectionException(const char* s)
    : Exception(s)
{
}

ReplyErrorException::ReplyErrorException(const std::string& s)
    : ReplyException(s)
{
}

ReplyErrorException::ReplyErrorException(const char* str)
    : ReplyException(str)
{
}

ReplyTypeException::ReplyTypeException(const char* str)
    : ReplyException(str)
{
}

ReplyTypeException::ReplyTypeException(const std::string& str)
    : ReplyException(str)
{
}

}
