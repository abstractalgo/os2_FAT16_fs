#pragma once

#include <stdio.h>
#include <windows.h>
#include "fs.h"
#include "newdisk.h"
#include "filemt.h"

class KernelFile
{
public:
    char write(BytesCnt, char* buffer);     // uradjeno
    BytesCnt read(BytesCnt, char* buffer);  //
    char seek(BytesCnt);                    // uradjeno

    BytesCnt filePos();                     // uradjeno
    char eof();                             // uradjeno
    BytesCnt getFileSize();                 // uradjeno
    char truncate();                        //
    ~KernelFile();                          //
private:
    uint16_t num;
    Entry entry;
    unsigned long caret;
    char mod;
    filemt::WaitingThread* threadtable;
    PathParser ppath;
    Disk& d;

    friend class FS;
    friend class KernelFS;
    KernelFile(Disk& _d);
};