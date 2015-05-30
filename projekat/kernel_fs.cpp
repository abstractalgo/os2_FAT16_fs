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
-------------- TODO -------------- TODO --------------
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
    tree(*disks[idx].disk);
    return res;
}

/*
-------------- TODO -------------- TODO --------------
proverava posotjanje fajla/foldera sa zadatom putanjom
*/
char KernelFS::doesExist(char* fname)
{
    return 0;
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
 //   // if doesExist parentFolder
 //   // napravi novi folder
	//PathIterator* pi = new PathIterator(dirname);
	///*if (!pi->isValid())
	//{
	//	delete pi;
	//	return 0;
	//}*/

	//Entry* pe = getEntry(pi->getParent());
	//Disk* d = disks[pi->disk() - 65].disk;
	//char* dir_name = pi->last();

	//if (pe == 0)
	//{
	//	delete pi;
	//	return 0;
	//}

	//// dodaj entry unutar klastera od roditelja
	//ClusterNo id;
	//ClusterNo offset = pe->size % 100;

	//// pun je klaster, treba uzeti novi
	//if (pe->size % 100 == 0 && pe->size>0)
	//{
	//	id = d->m_meta->freeNode;
	//	d->m_meta->freeNode = d->m_FAT[d->m_meta->freeNode];

	//	ClusterNo klid = pe->firstCluster;
	//	while (d->m_FAT[klid] != 0)
	//		klid = d->m_FAT[klid];
	//	d->m_FAT[klid] = id;
	//	d->m_FAT[id] = 0;
	//}
	//// ima mesta u klasteru
	//else
	//{
	//	id = pe->firstCluster;
	//	while (d->m_FAT[id] != 0)
	//		id = d->m_FAT[id];
	//}

	//// oslobodi jedan klaster za podatke novonapravljenog foldera
	//ClusterNo dataFolder = d->m_meta->freeNode;
	//d->m_meta->freeNode = d->m_FAT[d->m_meta->freeNode];
	//d->m_FAT[dataFolder] = 0;

	//// napravi entry
	//Entry* e = new Entry;
	//e->attributes = 0x02;
	//e->firstCluster = 0; //todo
	//e->size = 0;
	//memcpy(e->name, dir_name, sizeof(char)*strlen(dir_name));
	//e->firstCluster = dataFolder;

	//// upisi entry u klaster
	//char* data = new char[2048];
	//d->readCluster(id, data);
	//memcpy((Entry*)data + offset, e, sizeof(Entry));
	//d->writeCluster(id, data);
	//delete e;

	//delete dir_name;
	//delete pi;
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