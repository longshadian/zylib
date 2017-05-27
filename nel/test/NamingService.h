#pragma once

#include <ctime>
#include <iostream>
#include <string>

class NamingService
{
public:
    NamingService();
    ~NamingService();
    NamingService(const NamingService& rhs) = delete;
    NamingService& operator=(const NamingService& rhs) = delete;
    NamingService(NamingService&& rhs) = delete;
    NamingService& operator=(NamingService&& rhs) = delete;

    void start();
private:
};
