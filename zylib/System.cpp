#include "System.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <linux/unistd.h>     /* 包含调用 _syscallX 宏等相关信息*/
#include <sys/sysinfo.h>
#include <sys/statfs.h> 

DiskInfo SystemInfo::getServerDiskInfo()
{
	DiskInfo disk_info = { 0, 0 };

	struct statfs diskInfo;
	statfs("/", &diskInfo);
	uint64_t blocksize = diskInfo.f_bsize;							//每个block里包含的字节数 
	uint64_t totalsize = blocksize * diskInfo.f_blocks;				//总的字节数，f_blocks为block的数目 

	uint64_t freeDisk = diskInfo.f_bfree * blocksize;				//剩余空间的大小 
	//uint64_t availableDisk = diskInfo.f_bavail * blocksize;		//可用空间大小 

	disk_info.totalsize = totalsize >> 20;
	disk_info.freesize = freeDisk >> 20;

	return disk_info;
}

MemInfo SystemInfo::getServerMemInfo()
{
	MemInfo mem_info = { 0, 0 };

	struct sysinfo info;
	if (sysinfo(&info)) {
		return mem_info;
	}

	mem_info.totalram = info.totalram >> 20;
	mem_info.freeram = info.freeram >> 20;

	return mem_info;
}

