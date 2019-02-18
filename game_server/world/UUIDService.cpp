#include "UUIDService.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

UUIDService::UUIDService()
{
}

bool UUIDService::init()
{
    return true;
}

std::string UUIDService::newUUID()
{
    auto u = m_generator();
    return boost::uuids::to_string(u);
}
