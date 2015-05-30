#ifndef _os2_kfs_
#define _os2_kfs_

#include "fs.h"
#include "part.h"
#include "windows.h"
#include "newdisk.h"

struct DiskDesc
{
    Disk* disk;
    bool used;
};


class KernelFS
{
    DiskDesc disks[26];

    KernelFS();
	~KernelFS();

	char mount(Partition* partition);
	char unmount(char part);
	char format(char part);

	char doesExist(char* fname);
	File* open(char* fname, char mode);
	char deleteFile(char* fname);

	char createDir(char* dirname);
	char deleteDir(char* dirname);
	char readDir(char* dirname, EntryNum n, Entry &e);
	
	friend class FS;
};

#endif