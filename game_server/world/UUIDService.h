#ifndef _UUID_SERVICE_H_
#define _UUID_SERVICE_H_

#include <iostream>

#include <boost/uuid/uuid_generators.hpp>

class UUIDService
{
public:
    UUIDService();
    ~UUIDService() = default;

    bool init();

    std::string newUUID();
private:
    boost::uuids::random_generator m_generator;
};

#endif
