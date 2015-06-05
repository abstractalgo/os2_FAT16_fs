#include "kernel_file.h"

/*
-------------- TODO -------------- TODO --------------
*/
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

    // dodji do odgovarajuceg klastera gde je caret
    ClusterNo cid = entry.firstCluster;
    {
        BytesCnt _ = caret;
        while (_ > 2048)
        {
            _ -= 2048;
            cid = d.FAT[cid];
        }
    }

    // upisi
    BytesCnt left = cnt;
    BytesCnt _start=caret, _cnt=0;
    char* w_buffer = new char[2048];
    while (left > 0)
    {
        _start = (_start + _cnt) % 2048;
        _cnt = (left + _start > 2048) ? (left % 2048 - _start) : (left - caret);

        readCluster(d, cid, w_buffer);
        memcpy(w_buffer + _start, buffer + (cnt - left), _cnt);
        writeCluster(d, cid, w_buffer);

        cid = d.FAT[cid];
        left -= _cnt;
    }
    delete[] w_buffer;

    // uvecaj
    entry.size += cnt;
    return 0;
}

/*
-------------- TODO -------------- TODO --------------
*/
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

/*
-------------- TODO -------------- TODO --------------
*/
char KernelFile::truncate()
{
    // TODO
    return 0;
}

/*
-------------- TODO -------------- TODO --------------
*/
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