#include "kernel_fs.h"

KernelFS::KernelFS()
{
    
}

KernelFS::~KernelFS()
{
    
}

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
    // formatiranje particije na disku
    char res = ::format(*(disks[idx].disk));
    // re-ucitavanje podataka u memoriju
    delete disks[idx].disk;
    disks[idx].disk = new Disk(p);
    return res;
}

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
    dir.attributes = 0x03;
    dir.firstCluster = d.meta.rootDir;
    dir.size = d.meta.rootSize;

    bool fnd = false;
    for (uint8_t i = 0; i < ppath.partsNum; i++)
    {
        if (dir.size == 0 || (i==ppath.partsNum-1 && dir.attributes==0x01))
            return 0;
        Entry* entries = new Entry[dir.size];
        listDir(d, dir, entries);
        fnd = false;
        for (uint8_t eid = 0; eid < dir.size; eid++)
        {
            if (matchName(entries[eid], ppath.parts[i]))
            {
                dir = entries[eid];
                fnd = true;
                break;
            }
        }
        delete[] entries;
        if (!fnd) return 0;
    }

    return 1;
}

/*
-------------- TODO -------------- TODO --------------
otvara fajl ili pravi novi
*/
File* KernelFS::open(char* fname, char mode)
{

    return 0;
}

/*
-------------- TODO -------------- TODO --------------
brise fajl
*/
char KernelFS::deleteFile(char* fname)
{
    // obrisi ga iz foldera
    // nadovezi *free
	return 0;
}

/*
-------------- TODO -------------- TODO --------------
pravi folder
*/
char KernelFS::createDir(char* dirname)
{
    PathParser ppath;
    parse(ppath, dirname);
    int idx = ppath.disk - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

    Disk& d = *disks[idx].disk;
    Entry e;

    // nadji parent folder
    if (getEntry(d, e, combine(ppath, ppath.partsNum - 1)))
    {
        // napravi novi entry
        Entry newFolder;
        newFolder.attributes = 0x02;
        newFolder.firstCluster = allocate(d);
        d.FAT[newFolder.firstCluster] = 0;
        memcpy(newFolder.name, ppath.parts[ppath.partsNum - 1], SOC*strlen(ppath.parts[ppath.partsNum - 1]));
        newFolder.size = 0;

        // ima mesta u trenutnom klasteru
        if (e.size==0 && e.size % 102 != 0)
        {
            // nadji poslednji klaster
            ClusterNo cid = d.FAT[e.firstCluster];
            while (d.FAT[cid] != 0) cid = d.FAT[cid];
            // procitaj klaster
            char* w_buffer = new char[2048];
            Entry* entries = (Entry*)w_buffer;
            readCluster(d, cid, w_buffer);
            // mesto za smestanje entry-ja
            entries[e.size % 102] = newFolder;
            writeCluster(d, cid, w_buffer);
        }
    }
    
	return 1;
}

/*
-------------- TODO -------------- TODO --------------
brise folder
*/
char KernelFS::deleteDir(char* dirname)
{
    // obrisi ga iz parent foldera
    // nadovezi *free

    PathParser ppath;
    parse(ppath, dirname);
    int idx = ppath.disk - 65;
    if (idx<0 || idx>25 || false == disks[idx].used)
        return 0;

    Disk& d = *disks[idx].disk;
    Entry e;

    // nadji parent folder
    if (getEntry(d, e, combine(ppath, ppath.partsNum - 1)))
    {

    }
	return 0;
}

/*
-------------- TODO -------------- TODO --------------
otvara n-ti Entry unutar foldera
*/
char KernelFS::readDir(char* dirname, EntryNum n, Entry &e)
{
	/*Entry* pe = getEntry(dirname);
	if (pe == 0)
		return 0;
*/
	// TODO: imas klaster, vrati sad n-ti entry
	return 0;
}