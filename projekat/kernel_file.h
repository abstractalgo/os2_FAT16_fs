#pragma once

#include <stdio.h>
#include "fs.h"
#include "newdisk.h"
// mt
#include <queue>
#include "semaphore.h"

class KernelFile
{
public:
    char write(BytesCnt, char* buffer);     // uradjeno
    BytesCnt read(BytesCnt, char* buffer);  // uradjeno
    char seek(BytesCnt);                    // uradjeno

    BytesCnt filePos();                     // uradjeno
    char eof();                             // uradjeno
    BytesCnt getFileSize();                 // uradjeno
    char truncate();                        //
    ~KernelFile();                          // uradjeno (to test)
//private:

    Entry           entry;
    unsigned long   caret;
    char            mod;
    PathParser      ppath;
    Disk&           d;

    friend class FS;
    friend class KernelFS;
    KernelFile(Disk& _d);
};