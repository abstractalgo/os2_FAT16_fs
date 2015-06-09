#include "kernel_file.h"
#include "settings.h"

char KernelFile::write(BytesCnt cnt, char* buffer)
{
    ClusterNo cid = entry.firstCluster;
    

    // alociraj nov prostor ako treba mesta
    uint8_t num = 1;
    while (d.FAT[cid] != 0)
    {
        num++;
        cid = d.FAT[cid];
    }
    ClusterNo diff = cnt - (num*2048 - caret);
    if (cnt > num * 2048 - caret)
    {
        uint8_t ccnt = (diff + 2047) / 2048;
        for (uint8_t i = 0; i < ccnt; i++)
        {
            ClusterNo nc = allocate(d);
            d.FAT[cid] = nc;
            d.FAT[nc] = 0;
            cid = nc;
        }
    }

    // dodji do odgovarajuceg klastera gde je caret
    cid = entry.firstCluster;
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
        _cnt = (left + _start > 2048) ? (left-(left + _start)%2048) : (left);

        readCluster(d, cid, w_buffer);
        memcpy(buffer + (cnt - left), w_buffer + _start, _cnt);
        writeCluster(d, cid, w_buffer);

        cid = d.FAT[cid];
        left -= _cnt;
    }
    delete[] w_buffer;

    // uvecaj
    entry.size += (caret+cnt-entry.size);
    caret += cnt;
    return 1;
}

BytesCnt KernelFile::read(BytesCnt cnt, char* buffer)
{
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
    BytesCnt left = cnt > entry.size - caret ? entry.size - caret : cnt;
    BytesCnt read = left;
    BytesCnt _start = caret, _cnt = 0;
    char* w_buffer = new char[2048];
    while (left > 0)
    {
        _start = (_start + _cnt) % 2048;
        _cnt = (left + _start > 2048) ? (left - ((left + _start) % 2048)) : (left);

        readCluster(d, cid, w_buffer);
        memcpy(buffer + (read - left), w_buffer + _start, _cnt);

        cid = d.FAT[cid];
        left -= _cnt;
    }
    delete[] w_buffer;

    caret += read;

    return read;
}

char KernelFile::seek(BytesCnt cnt)
{
    if (entry.size <= cnt)
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

KernelFile::~KernelFile()
{
    // ako jos neko ceka na fajl, onda samo prosledi objekat
    bool awaits = false;
    if (waitQueue.size() > 0)
    {
        signal(waitQueue.front());
        waitQueue.pop();
        return;
    }

    // izbrisi ga iz liste otvorenih fajlova
    d.filetable.erase(std::remove(d.filetable.begin(), d.filetable.end(), this), d.filetable.end());

    // upis entry-ja nazad
    Entry dir;
    getEntry(d, dir, combine(ppath, ppath.partsNum - 1));
    Entry* ents = new Entry[dir.size];
    listDir(d, dir, ents);

    // nadji
    uint16_t idx = ~0;
    for (uint16_t i = 0; i < dir.size; i++)
    {
        if (strncmp(ents[i].name, entry.name, 8)==0 && strncmp(ents[i].ext, entry.ext, 3)==0)
        {
            idx = i;
            break;
        }
    }
    delete[] ents;

    // upisi ga u memoriju
    ClusterNo cid = dir.firstCluster;
    {
        uint16_t _ = idx;
        while (idx>102)
        {
            _ -= 102;
            cid = d.FAT[cid];
        }
    }
    char w_buffer[2048];
    Entry* e_buffer = (Entry*)w_buffer;
    readCluster(d, cid, w_buffer);
    e_buffer[idx] = entry;
    writeCluster(d, cid, w_buffer);

    // otpustanje MT
    /*filemt::release_file_access(threadtable);
    if (!threadtable)
        filemt::unregister_fopen(d.filetable, this);

    if (!d.filetable)
        signal(d.un_mountS);*/
}

// private

KernelFile::KernelFile(Disk& _d)
    : caret(0)
    , d(_d)
{
}