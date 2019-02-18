#include "System.h"

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <linux/unistd.h>     /* �������� _syscallX ��������Ϣ*/
#include <sys/sysinfo.h>
#include <sys/statfs.h> 

DiskInfo SystemInfo::getServerDiskInfo()
{
	DiskInfo disk_info = { 0, 0 };

	struct statfs diskInfo;
	statfs("/", &diskInfo);
	uint64_t blocksize = diskInfo.f_bsize;							//ÿ��block��������ֽ��� 
	uint64_t totalsize = blocksize * diskInfo.f_blocks;				//�ܵ��ֽ�����f_blocksΪblock����Ŀ 

	uint64_t freeDisk = diskInfo.f_bfree * blocksize;				//ʣ��ռ�Ĵ�С 
	//uint64_t availableDisk = diskInfo.f_bavail * blocksize;		//���ÿռ��С 

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

