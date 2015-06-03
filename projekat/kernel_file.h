#pragma once

#include <stdio.h>
#include <windows.h>
#include "fs.h"
#include "newdisk.h"
#include "filemt.h"

class KernelFile
{
public:
    char write(BytesCnt, char* buffer);
    BytesCnt read(BytesCnt, char* buffer);
    char seek(BytesCnt);

    BytesCnt filePos();
    char eof();
    BytesCnt getFileSize();
    char truncate();
    ~KernelFile();
private:

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