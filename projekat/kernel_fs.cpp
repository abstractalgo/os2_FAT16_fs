#include "kernel_fs.h"

KernelFS::KernelFS()
{
    
}

KernelFS::~KernelFS()
{
    
}

// -----------------------------------------------------------------------------

/*
montira particiju/disk
*/
char KernelFS::mount(Partition* partition)
{
    for (int i = 0; i < 26; i++)
    {
        if (false == disks[i].used)
        {
            disks[i].used = true;
            disks[i].disk = new Disk(partition);
            return i+65;
        }
    }
    return 0;
}

/*
demontira particiju/disk
*/
char KernelFS::unmount(char part)
{
    int idx = part-65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;
    // TODO: blokiranje niti dok se ne zatvore svi fajlovi                      
    disks[idx].used = false;
    delete disks[idx].disk;
    disks[idx].disk = 0;
	return 1;
}

/*
formatira particiju/disk
*/
char KernelFS::format(char part)
{
    int idx = part-65;
    if (idx<0 || idx>25 || false==disks[idx].used)
	    return 0;

    // TODO: blokiranje niti dok se ne zatvore svi fajlovi

    Partition* p = disks[idx].disk->partition;
    Disk& _d = *(disks[idx].disk);

    // formatiranje particije na disku
    char w_buffer[2048];
    ClusterNo* buffer = (ClusterNo*)w_buffer;

    // racunanje velicine
    ClusterNo totalClsCnt = _d.partition->getNumOfClusters();
    ClusterNo dataClsCnt = ((totalClsCnt - 1) * 512) / 513;
    ClusterNo FATClsCnt = totalClsCnt - 1 - dataClsCnt;

    //printf("************\nTotal: %d\nData: %d\nFAT: %d\n************\n", totalClsCnt, dataClsCnt, FATClsCnt);

    // upis meta podataka
    buffer[0] = 1;          // free node
    buffer[1] = dataClsCnt; // FAT size
    buffer[2] = 0;          // root dir
    buffer[3] = 0;          // root size
    _d.partition->writeCluster(0, w_buffer);

    // formatiranje FAT tabele
    ClusterNo left = dataClsCnt;
    for (ClusterNo cid = 0; cid < FATClsCnt; cid++)
    {
        for (ClusterNo i = 0; i < 512; i++)
        {
            ClusterNo idx = (cid * 512) + i;
            buffer[i] = idx + 1;
            if (0 == idx)
                buffer[0] = 0;
            if (idx == dataClsCnt)
            {
                buffer[i] = 0;
                break;
            }
        }
        _d.partition->writeCluster(1 + cid, w_buffer);
    }

    // re-ucitavanje podataka u memoriju
    delete disks[idx].disk;
    disks[idx].disk = new Disk(p);

    return 1;
}

// -----------------------------------------------------------------------------

/*
proverava posotjanje fajla/foldera sa zadatom putanjom
*/
char KernelFS::doesExist(char* fname)
{
    PathParser ppath;
    parse(ppath, fname);
    int idx = ppath.disk - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

    Disk& d = *disks[idx].disk;

    Entry dir;
    return getEntry(d, dir, fname);
}

/*
pravi folder
*/
char KernelFS::createDir(char* dirname)
{
    int idx = dirname[0] - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

    Disk& d = *disks[idx].disk;
    return createEntry(d, dirname);
}

/*
brise folder
*/
char KernelFS::deleteDir(char* dirname)
{
    uint8_t idx = dirname[0] - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

    Disk& d = *disks[idx].disk;
    return deleteEntry(d, dirname);
}

/*
otvara n-ti Entry unutar foldera
*/
char KernelFS::readDir(char* dirname, EntryNum n, Entry &e)
{
    PathParser ppath;
    parse(ppath, dirname);
    int idx = ppath.disk - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

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
            return 1;
        }
    }
	return 0;
}

// -----------------------------------------------------------------------------

/*
-------------- TODO -------------- TODO --------------
otvara fajl ili pravi novi
*/
File* KernelFS::open(char* fname, char mode)
{
    PathParser ppath;
    parse(ppath, fname);
    int idx = ppath.disk - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

    Disk& d = *disks[idx].disk;

    // proverava da li je vec otvaran fajl
    bool __access = false;
    OpenedFilesTable temp = d.filetable;
    char* name1, *name2;
    name1 = combine(ppath, ppath.partsNum);
    while (temp)
    {
        name2 = combine(temp->file->ppath, temp->file->ppath.partsNum);
        if (0 == strcmp(name1, name2))
        {
            // ranije otvoren fajl, stavi se na cekanje
            __access = true;
            break;
        }
        delete[] name2;
        temp = temp->next;
    }
    delete[] name1;

    // otvaranje fajla
    File* f = new File;
    f->myImpl = new KernelFile(d);
    Entry e;
    
    // READ
    if ('r' == mode)
    {
        if(!doesExist(fname))
            return 0;
        getEntry(d, e, fname);
        f->myImpl->caret = 0;
    }
    // WRITE
    else if ('w' == mode)
    {
        // pravi novi fajl
        // ako postoji fajl istog imena i zatvoren je -> obrisi ga TODO
        createEntry(d, fname);
        getEntry(d, e, fname);
        f->myImpl->caret = 0;
    }
    // APPEND
    else if ('a' == mode)
    {
        if (!doesExist(fname))
            return 0;
        getEntry(d, e, fname);
        f->myImpl->caret = e.size;
    }
    else
    {
        return 0;
    }

    // povezi sa entry-jem
    f->myImpl->entry = e;

    // modalitet
    f->myImpl->mod = mode;

    // path
    parse(f->myImpl->ppath, fname);

    if (__access)
    {
        filemt::register_fopen(d.filetable, f->myImpl);
        filemt::request_file_access(temp->file->threadtable);
    }

    return f;
}

/*
-------------- TODO -------------- TODO --------------
brise fajl
*/
char KernelFS::deleteFile(char* fname)
{
    // veoma slicno kao i brisanje foldera ali treba da je MT (tj, ne sme biti otvoren)
    uint8_t idx = fname[0] - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

    Disk& d = *disks[idx].disk;
    return deleteEntry(d, fname);
}