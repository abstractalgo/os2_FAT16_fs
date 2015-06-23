#include "kernel_fs.h"

KernelFS::KernelFS()
{
	mutex = CreateMutex(NULL, FALSE, NULL);
}

KernelFS::~KernelFS()
{
	CloseHandle(mutex);
}

// -----------------------------------------------------------------------------

/*
montira particiju/disk
*/
char KernelFS::mount(Partition* partition)
{
	WaitForSingleObject(mutex, INFINITE);

    for (int i = 0; i < 26; i++)
    {
        if (false == disks[i].used)
        {
            disks[i].used = true;
            disks[i].disk = new Disk(partition);

			ReleaseMutex(mutex);
            return i+65;
        }
    }

	ReleaseMutex(mutex);
    return 0;
}

/*
demontira particiju/disk
*/
char KernelFS::unmount(char part)
{
	WaitForSingleObject(mutex, INFINITE);

    int idx = part-65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

    Disk& _d = *(disks[idx].disk);

	if (_d.filetable.size() > 0 || _d.formatRequest)
	{
		_d.unmountRequest = true;
		SignalObjectAndWait(mutex, _d.unmountMutex, INFINITE, FALSE);
		WaitForSingleObject(mutex, INFINITE);
	}

    disks[idx].used = false;
    delete disks[idx].disk;
    disks[idx].disk = 0;

	ReleaseMutex(mutex);
	return 1;
}

/*
formatira particiju/disk
*/
char KernelFS::format(char part)
{
	WaitForSingleObject(mutex, INFINITE);

    int idx = part-65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

    Partition* p = disks[idx].disk->partition;
    Disk& _d = *(disks[idx].disk);

	if (_d.unmountRequest || _d.formatRequest)
	{
		ReleaseMutex(mutex);
		return 0;
	}

	if (_d.filetable.size() > 0)
	{
		_d.formatRequest = true;
		SignalObjectAndWait(mutex, _d.formatMutex, INFINITE, FALSE);
		WaitForSingleObject(mutex, INFINITE);
	}

    // formatiranje particije na disku
    char w_buffer[2048];
    ClusterNo* buffer = (ClusterNo*)w_buffer;

    // racunanje velicine
    ClusterNo totalClsCnt = _d.partition->getNumOfClusters();
    ClusterNo dataClsCnt = ((totalClsCnt - 1) * 512) / 513;
    ClusterNo FATClsCnt = totalClsCnt - 1 - dataClsCnt;

    _d.meta.freeNode    = 1;
    _d.meta.fatSize     = dataClsCnt;
    _d.meta.rootDir     = 0;
    _d.meta.rootSize    = 0;

    _d.FAT = new ClusterNo[_d.meta.fatSize];
    for (ClusterNo i = 0; i < _d.meta.fatSize; i++)
        _d.FAT[i] = i+1;

    _d.FAT[0] = 0;
    _d.FAT[_d.meta.fatSize-1] = 0;

	_d.formatRequest = false;
	if (_d.filetable.size() == 0 && _d.unmountRequest)
	{
		ReleaseMutex(_d.unmountMutex);
	}

	ReleaseMutex(mutex);
    return 1;
}

// -----------------------------------------------------------------------------

/*
proverava posotjanje fajla/foldera sa zadatom putanjom
*/
char KernelFS::doesExist(char* fname)
{
	WaitForSingleObject(mutex, INFINITE);

    PathParser ppath;
    parse(ppath, fname);
    int idx = ppath.disk - 65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

    Disk& d = *disks[idx].disk;

    Entry dir;
    bool ret = getEntry(d, dir, fname);

	ReleaseMutex(mutex);
	return ret;
}

/*
pravi folder
*/
char KernelFS::createDir(char* dirname)
{
	WaitForSingleObject(mutex, INFINITE);

    int idx = dirname[0] - 65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

    Disk& d = *disks[idx].disk;
    bool ret = createEntry(d, dirname);

	ReleaseMutex(mutex);
	return ret;
}

/*
brise folder
*/
char KernelFS::deleteDir(char* dirname)
{
	WaitForSingleObject(mutex, INFINITE);

    uint8_t idx = dirname[0] - 65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

    Disk& d = *disks[idx].disk;
    bool ret = deleteEntry(d, dirname);

	ReleaseMutex(mutex);
	return ret;
}

/*
otvara n-ti Entry unutar foldera
*/
char KernelFS::readDir(char* dirname, EntryNum n, Entry &e)
{
	WaitForSingleObject(mutex, INFINITE);

    PathParser ppath;
    parse(ppath, dirname);
    int idx = ppath.disk - 65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

    Disk& d = *disks[idx].disk;
    Entry dir;

    // nadji folder gde treba napraviti novi entry
    if (getEntry(d, dir, dirname))
    {
        Entry* entries = new Entry[dir.size];
        listDir(d, dir, entries);
        if (n >= 0 && n < dir.size)
        {
            e = entries[n];
            delete[] entries;

			ReleaseMutex(mutex);
            return 1;
        }
		else
		{
			ReleaseMutex(mutex);
			return 2;
		}
    }

	ReleaseMutex(mutex);
	return 0;
}

// -----------------------------------------------------------------------------

/*
otvara fajl ili pravi novi
*/
File* KernelFS::open(char* fname, char mode)
{
	WaitForSingleObject(mutex, INFINITE);

    PathParser ppath;
    
	if (!parse(ppath, fname))
	{
		ReleaseMutex(mutex);
		return 0;
	}

    int idx = ppath.disk - 65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

    Disk& d = *disks[idx].disk;

//    if (d.un_mountB)
//        return 0;

    File* f = new File();
    f->myImpl = new KernelFile(d, mutex);
	Entry e;

	// fajl nije otvoren
	if (!isOpen(d, fname))
	{
		if (!doesExist(fname) && (mode == 'r' || mode == 'a'))
		{
			ReleaseMutex(mutex);
			return 0;
		}
		else
		{
			d.filetable[std::string(fname)] = std::queue<HANDLE>();
		}
	}
	// fajl od ranije otvoren
	else
	{
		waitFile(d, std::string(fname), mutex);
	}
    
    // READ
    if ('r' == mode)
    {
		if (!doesExist(fname))
		{
			ReleaseMutex(mutex);
			return 0;
		}
        getEntry(d, e, fname);
        f->myImpl->caret = 0;
    }
    // WRITE
    else if ('w' == mode)
    {
        // pravi novi fajl
        // ako postoji fajl istog imena i zatvoren je -> obrisi ga TODO
		//if (!doesExist(fname))
			//createEntry(d, fname);
		if (doesExist(fname))
			deleteEntry(d, fname);

		createEntry(d, fname);
        getEntry(d, e, fname);
        f->myImpl->caret = 0;
		//f->myImpl->truncate();
    }
    // APPEND
	else if ('a' == mode)
	{
		if (!doesExist(fname))
		{
			ReleaseMutex(mutex);
			return 0;
		}
        getEntry(d, e, fname);
        f->myImpl->caret = e.size;
    }
    else
    {
		ReleaseMutex(mutex);
        return 0;
    }

    // povezi sa entry-jem
    f->myImpl->entry = e;

    // modalitet
    f->myImpl->mod = mode;

    // path
    parse(f->myImpl->ppath, fname);

	ReleaseMutex(mutex);
    return f;
}

/*
brise fajl
*/
char KernelFS::deleteFile(char* fname)
{
	WaitForSingleObject(mutex, INFINITE);

	PathParser ppath;
	parse(ppath, fname);
	// veoma slicno kao i brisanje foldera ali treba da je MT (tj, ne sme biti otvoren)
	uint8_t idx = ppath.disk - 65;
	if (idx<0 || idx>25 || false == disks[idx].used)
	{
		ReleaseMutex(mutex);
		return 0;
	}

	Disk& d = *disks[idx].disk;

	// proveri da li je otvoren fajl
	bool found = false;
	//char* name1, *name2;
	//uint8_t fcnt = d.filetable.size();
	//name1 = combine(ppath, ppath.partsNum);


	//for (std::map<KernelFile*, std::queue<Semaphore>*>::iterator it = d.filetable.begin(); it != d.filetable.end(); ++it)
	//{
	//    name2 = combine(it->first->ppath, it->first->ppath.partsNum);
	//    if (0 == strcmp(name1, name2))
	//    {
	//        // otvoren fajl
	//        found = true;
	//        break;
	//    }
	//    delete[] name2;
	//}
	//delete[] name1;

	if (found)
	{
		ReleaseMutex(mutex);
		return 0;
	}

	ReleaseMutex(mutex);
    return deleteEntry(d, fname);
}

void writefopens()
{
    putchar('\n');
    /*for (int i = 0; i < 26; i++)
    {
        if (!FS::myImpl->disks[i].used)
            continue;
        Disk& d = *FS::myImpl->disks[i].disk;
        printf("%c (", 'A' + i);
        uint8_t fcnt = d.filetable.size();
        for (std::map<KernelFile*, std::queue<Semaphore>*>::iterator it = d.filetable.begin(); it != d.filetable.end(); ++it)
        {
            Entry& _e =it->first->entry;
            printf(" %.8s%c%.3s [%d] ", _e.name, _e.attributes == 0x01 ? '.' : '\0', _e.attributes == 0x01 ? _e.ext : "",it->second->size());
        }
        printf(")\n");
    }*/
}

