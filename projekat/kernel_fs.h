#ifndef _os2_kfs_
#define _os2_kfs_

#include <Windows.h>
#include "fs.h"
#include "part.h"
#include "disk.h"
#include "file.h"
#include "kernel_file.h"

class KernelFS
{
    struct DiskDesc
    {
        Disk* disk;
        bool used;
        DiskDesc()
            : used(false)
            , disk(NULL)
        {}
    } disks[26];

    HANDLE mutex;

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

public:
    KernelFS();
    ~KernelFS();
};

#endif