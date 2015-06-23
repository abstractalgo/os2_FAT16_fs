#include "kernel_file.h"
#include "settings.h"

char KernelFile::write(BytesCnt cnt, char* buffer)
{
    ClusterNo cid = entry.firstCluster;

	if (caret >= 2047)
	{
		int dummy = 1;
	}
    

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
		WaitForSingleObject(mutex, INFINITE);

		ClusterNo ccnt = (diff + 2047) / 2048;
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
    {
        BytesCnt _ = caret;
        while (_ >= 2048)
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
        _cnt = (left + _start > 2048) ? (2048-_start) : (left);

        readCluster(d, cid, w_buffer);
		memcpy(w_buffer + _start, buffer + (cnt - left), _cnt);
        writeCluster(d, cid, w_buffer);

        cid = d.FAT[cid];
        left -= _cnt;
    }
    delete[] w_buffer;

    // uvecaj
//    entry.size += (caret+cnt-entry.size);
	caret += cnt;
	entry.size = (entry.size < caret) ? caret : entry.size;
    return 1;
}

BytesCnt KernelFile::read(BytesCnt cnt, char* buffer)
{
    // dodji do odgovarajuceg klastera gde je caret
	if (caret >= 2047)
	{
		int dummy = 1;
	}

    ClusterNo cid = entry.firstCluster;
    {
        BytesCnt _ = caret;
        while (_ >= 2048)
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
        _cnt = (left + _start > 2048) ? (2048-_start) : (left);

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
    // dodji do klaster od kog treba oslobadjati
	ClusterNo cid = entry.firstCluster;
	ClusterNo pid = 0;
	{
		BytesCnt _ = caret;
		while (_ > 2048)
		{
			_ -= 2048;
			cid = d.FAT[cid];
		}
		
		if (caret % 2048 != 0)
		{
			pid = cid;
			cid = d.FAT[cid];
		}
		d.FAT[pid] = 0;
	}
	
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

// private

KernelFile::KernelFile(Disk& _d, HANDLE mutex)
    : caret(0)
    , d(_d)
	, mutex(mutex)
{
}