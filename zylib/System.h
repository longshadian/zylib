#ifndef __SYSTEM_HEAD__
#define __SYSTEM_HEAD__

#include <iostream>
#include <memory>


struct DiskInfo
{
	uint64_t totalsize;
	uint64_t freesize;
};

struct MemInfo
{
	uint64_t totalram;
	uint64_t freeram;
};

class SystemInfo
{
public:
	static MemInfo getServerMemInfo();
	static DiskInfo getServerDiskInfo();

};

#endif

