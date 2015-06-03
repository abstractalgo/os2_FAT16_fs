#include "kernel_file.h"

char KernelFile::write(BytesCnt cnt, char* buffer)
{
    // alociraj nov prostor ako treba mesta
    ClusterNo diff = cnt - (entry.size - caret);
    if (diff > 0)
    {
        uint8_t ccnt = (diff + 2047) / 2048;
        ClusterNo cid = entry.firstCluster;
        while (d.FAT[cid] != 0) cid = d.FAT[cid];
        for (uint8_t i = 0; i < ccnt; i++)
        {
            ClusterNo nc = allocate(d);
            d.FAT[cid] = nc;
            d.FAT[nc] = 0;
            cid = nc;
        }
    }

    //
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

KernelFile::KernelFile(Disk& _d)
    : caret(0)
    , d(_d)
{
}