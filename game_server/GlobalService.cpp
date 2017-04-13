#include "GlobalService.h"

#include "DataBaseService.h"

bool Global::init()
{
    return true;
}

Global& Global::instance()
{
    static Global instance_;
    return instance_;
}

DataBaseService& Global::getDataBaseService()
{
    return *instance().m_database_service;
}
