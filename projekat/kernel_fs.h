#ifndef _os2_kfs_
#define _os2_kfs_

#include "fs.h"
#include "part.h"
#include "windows.h"
#include "newdisk.h"

class KernelFS
{
public:
    struct DiskDesc
    {
        Disk* disk;
        bool used;
        DiskDesc()
            : used(false)
            , disk(0)
        {}
    } disks[26];

    KernelFS();
	~KernelFS();

	char mount(Partition* partition);                   // uradjeno (bez MT)
	char unmount(char part);                            // uradjeno (bez MT)
	char format(char part);                             // uradjeno (bez MT)

	char doesExist(char* fname);                        // uradjeno
	File* open(char* fname, char mode);                 // (bez MT)
	char deleteFile(char* fname);                       // 

	char createDir(char* dirname);                      // uradjeno
	char deleteDir(char* dirname);                      // 
	char readDir(char* dirname, EntryNum n, Entry &e);  // uradjeno

	friend class FS;
};

#endif