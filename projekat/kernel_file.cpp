#include "kernel_file.h"
#include "settings.h"

char KernelFile::write(BytesCnt cnt, char* buffer)
{
    ClusterNo cid = entry.firstCluster;

    // alociraj nov prostor ako treba mesta
    ClusterNo ccnt = (entry.size + 2047) / 2048;
    if (ccnt == 0) ccnt = 1;
    while (d.FAT[cid] != 0)
    {
        cid = d.FAT[cid];
    }
    ClusterNo diff = 0;
    if (cnt > ccnt * 2048 - caret)
    {
        diff = cnt - (ccnt * 2048 - caret);
        ClusterNo ccnt = (diff + 2047) / 2048;

        WaitForSingleObject(mutex, INFINITE);
        
        for (ClusterNo i = 0; i < ccnt; i++)
        {
            ClusterNo nc = allocate(d);
            d.FAT[cid] = nc;
            d.FAT[nc] = 0;
            cid = nc;
        }

        ReleaseMutex(mutex);
    }

    // dodji do odgovarajuceg klastera gde je caret
    cid = entry.firstCluster;
    { BytesCnt _ = caret; while (_ >= 2048) {  _ -= 2048; cid = d.FAT[cid]; } }

    // upisi
    BytesCnt left = cnt;
    BytesCnt _start=caret, _cnt=0;
    while (left > 0)
    {
        _start = (_start + _cnt) % 2048;
        _cnt = (left + _start > 2048) ? (2048-_start) : (left);

        // upis manje od celog klastera
        if (_cnt < 2048)
        {
            if (cid != cWRi)
            {
                if (cWRd)
                    writeCluster(d, cWRi, cWRb);
                readCluster(d, cid, cWRb);
                cWRd = false;
                cWRi = cid;
            }
            memcpy(cWRb + _start, buffer + (cnt - left), _cnt);
            cWRd = true;
        }
        // upis tacno celog klastera
        else
        {
            writeCluster(d, cid, buffer + (cnt - left));
            cWRi = ~0;
            cWRd = false;
        }

        cid = d.FAT[cid];
        left -= _cnt;
    }

    // uvecaj
    caret += cnt;
    entry.size = (entry.size < caret) ? caret : entry.size;
    return 1;
}

extern char p1;

BytesCnt KernelFile::read(BytesCnt cnt, char* buffer)
{
    // dodji do odgovarajuceg klastera gde je caret
    ClusterNo cid = entry.firstCluster;
    { BytesCnt _ = caret; while (_ >= 2048) { _ -= 2048; cid = d.FAT[cid]; } }

    // upisi preostalo
    BytesCnt left = cnt > entry.size - caret ? entry.size - caret : cnt;
    BytesCnt read = left;
    BytesCnt _start = caret, _cnt = 0;
    while (left > 0)
    {
        // racunaj offset-e
        _start = (_start + _cnt) % 2048;
        _cnt = (left + _start > 2048) ? (2048-_start) : (left);

        // ako nije vec ucitan klaster
        if (cid != cRDi)
        {
            readCluster(d, cid, cRDb);
            cRDi = cid;
        }
        memcpy(buffer + (read - left), cRDb + _start, _cnt);

        cid = d.FAT[cid];
        left -= _cnt;
    }

    caret += read;

    return read;
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
    // dodji do klaster od kog treba oslobadjati
    ClusterNo cid = entry.firstCluster;
    ClusterNo pid = 0;
    {
        BytesCnt _ = caret;
        while (_ >= 2048)
        {
            _ -= 2048;
            cid = d.FAT[cid];
        }
    }

    cid = d.FAT[cid];
    
    // ako treba da oslobodi nesto
    if (cid != 0)
    {
        ClusterNo oldFR = d.meta.freeNode;
        d.meta.freeNode = cid;
        while (d.FAT[cid] != 0)
            cid = d.FAT[cid];
        d.FAT[cid] = oldFR;
    }
        
    // promeni velicinu
    entry.size = caret;
    
    return 0;
}

KernelFile::~KernelFile()
{
    // upis entry-ja nazad
    WaitForSingleObject(mutex, INFINITE);

    if (cWRd) writeCluster(d, cWRi, cWRb);
    
    Entry dir;
    std::string parentPath = path.substr(0, path.find_last_of("\\"));
    getEntry(d, dir, (char*)parentPath.c_str());
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
        //uint16_t _ = idx;
        while (idx>102)
        {
            idx -= 102;
            cid = d.FAT[cid];
        }
    }
    char w_buffer[2048];
    Entry* e_buffer = (Entry*)w_buffer;
    readCluster(d, cid, w_buffer);
    e_buffer[idx] = entry;
    writeCluster(d, cid, w_buffer);

    // zatvaranje fajla
    closeFile(d, this);

    ReleaseMutex(mutex);    
}

KernelFile::KernelFile(Disk& _d, HANDLE mutex)
    : caret(0)
    , d(_d)
    , mutex(mutex)
    , cRDi(~0)
    , cWRi(~0)
    , cWRd(false)
{
}