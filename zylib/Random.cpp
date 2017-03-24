#include "Random.h"

namespace zylib {

std::default_random_engine g_engine;

void randomEngineInit()
{
    //std::random_device rd; g_engine = std::default_random_engine(rd);
    g_engine = std::default_random_engine(static_cast<unsigned int>(time(nullptr)));
}

//////////////////////////////////////////////////////////////////////////
}
