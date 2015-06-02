#include "stdio.h"
#include <windows.h>
#include "fs.h"
#include "kernel_file.h"

char KernelFile::write(BytesCnt cnt, char* buffer)
{
    // TODO
    return 0;
}

BytesCnt KernelFile::read(BytesCnt cnt, char* buffer)
{
    // TODO
    return 0;
}

char KernelFile::seek(BytesCnt cnt)
{
    if (entry.size < cnt)
        return 0;

    caret = cnt;
    return 1;
}

BytesCnt KernelFile::filePos()
{
    return caret;
}

char KernelFile::eof()
{
    return caret == entry.size;
}

BytesCnt KernelFile::getFileSize()
{
    return entry.size;
}

char KernelFile::truncate()
{
    // TODO
    return 0;
}

KernelFile::~KernelFile()
{
    // TODO
}

// private

KernelFile::KernelFile()
{
    // TODO
}