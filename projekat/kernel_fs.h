#ifndef _os2_kfs_
#define _os2_kfs_

#include "fs.h"
#include "part.h"
#include "windows.h"
#include "newdisk.h"
#include "file.h"
#include "kernel_file.h"
#include "filemt.h"

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

	char mount(Partition* partition);                   // uradjeno
	char unmount(char part);                            // uradjeno (bez MT)
	char format(char part);                             // uradjeno (bez MT)

	char doesExist(char* fname);                        // uradjeno
	File* open(char* fname, char mode);                 // 
	char deleteFile(char* fname);                       // uradjeno (bez MT)

	char createDir(char* dirname);                      // uradjeno
	char deleteDir(char* dirname);                      // uradjeno
	char readDir(char* dirname, EntryNum n, Entry &e);  // uradjeno

	friend class FS;
};

#endif