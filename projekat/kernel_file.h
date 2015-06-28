#pragma once

#include <Windows.h>
#include <stdio.h>
#include "fs.h"
#include "disk.h"

class KernelFile
{
public:
    char        write(BytesCnt, char* buffer);
    BytesCnt    read(BytesCnt, char* buffer);
    char        seek(BytesCnt);

    BytesCnt    filePos();
    char        eof();
    BytesCnt    getFileSize();
    char        truncate();        // TODO

    ~KernelFile();
private:
    KernelFile(Disk& _d, HANDLE mutex);

    Entry           entry;
    unsigned long   caret;
    char            mod;
    std::string     path;
    Disk&           d;
    HANDLE          mutex;

    // read cache
    char        cRDb[BUFF_SIZE];
    ClusterNo   cRDi;

    // write cache
    char        cWRb[BUFF_SIZE];
    ClusterNo   cWRi;
    bool        cWRd;

    friend class FS;
    friend class KernelFS;
    friend bool closeFile(Disk& _d, KernelFile* _file);
    
};