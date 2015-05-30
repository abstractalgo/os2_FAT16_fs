#include "kernel_fs.h"
#include "pathparser.h"

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
    return disks[idx].disk->format();
}

/*
proverava posotjanje fajla/foldera sa zadatom putanjom
*/
char KernelFS::doesExist(char* fname)
{
    return getEntry(fname)!=0;
}

/*
-------------- TODO -------------- TODO --------------
otvara fajl ili pravi novi
*/
File* KernelFS::open(char* fname, char mode)
{
    // if doesExist, vrati Entry
    Entry* pe = getEntry(fname);
	File* f = 0;

	// fajl ne postoji, zato treba napraviti novi
	if (pe == 0)
	{

	}
	// fajl posotji, treba ga vratiti
	else
	{

	}
	
	return f;
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
    // if doesExist parentFolder
    // napravi novi folder
	PathIterator* pi = new PathIterator(dirname);
	/*if (!pi->isValid())
	{
		delete pi;
		return 0;
	}*/

	Entry* pe = getEntry(pi->getParent());
	Disk* d = disks[pi->disk() - 65].disk;
	char* dir_name = pi->last();

	if (pe == 0)
	{
		delete pi;
		return 0;
	}

	// dodaj entry unutar klastera od roditelja
	ClusterNo id;
	ClusterNo offset = pe->size % 100;

	// pun je klaster, treba uzeti novi
	if (pe->size % 100 == 0 && pe->size>0)
	{
		id = d->m_meta->freeNode;
		d->m_meta->freeNode = d->m_FAT[d->m_meta->freeNode];

		ClusterNo klid = pe->firstCluster;
		while (d->m_FAT[klid] != 0)
			klid = d->m_FAT[klid];
		d->m_FAT[klid] = id;
		d->m_FAT[id] = 0;
	}
	// ima mesta u klasteru
	else
	{
		id = pe->firstCluster;
		while (d->m_FAT[id] != 0)
			id = d->m_FAT[id];
	}

	// oslobodi jedan klaster za podatke novonapravljenog foldera
	ClusterNo dataFolder = d->m_meta->freeNode;
	d->m_meta->freeNode = d->m_FAT[d->m_meta->freeNode];
	d->m_FAT[dataFolder] = 0;

	// napravi entry
	Entry* e = new Entry;
	e->attributes = 0x02;
	e->firstCluster = 0; //todo
	e->size = 0;
	memcpy(e->name, dir_name, sizeof(char)*strlen(dir_name));
	e->firstCluster = dataFolder;

	// upisi entry u klaster
	char* data = new char[2048];
	d->readCluster(id, data);
	memcpy((Entry*)data + offset, e, sizeof(Entry));
	d->writeCluster(id, data);
	delete e;

	delete dir_name;
	delete pi;
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
	Entry* pe = getEntry(dirname);
	if (pe == 0)
		return 0;

	// TODO: imas klaster, vrati sad n-ti entry
	return 0;
}

/*
kopira podatke u novi Entry pokazivac
*/
Entry* KernelFS::copyEntry(Entry& e)
{
    Entry* pe = new Entry;
	memcpy(pe, &e, sizeof(Entry));
    return pe;
}

/*
poredi string i ime Entry-ja
*/
bool KernelFS::matchName(Entry& e, char* name)
{
    int i = 0, j = 0;
    bool tacka = false;
    while (name[i] != '\0')
    {
        if (name[i] == '.')
        {
            tacka = true;
            j = 0;
            i++;
        }
        if (( tacka && e.ext[j]  != name[i]) ||
			(!tacka && e.name[j] != name[i]) )
                return false;
        j++;
        i++;
    }
    return true;
}

/*
vraca pokazivac na Entry sa zadatom putanjom,
ako Entry ne postoji pokazivac je NULL
*/
Entry* KernelFS::getEntry(char* path)
{
	if (!disks[path[0] - 65].used)
		return 0;

	PathIterator* pi	= new PathIterator(path);
	Disk* d				= disks[pi->disk() - 65].disk;

	// ako nije validan, iskoci
	if (!pi->isValid())
		return 0;

	Entry* currEntry = new Entry;
	char* name = 0;

	// dohvati root
	currEntry->attributes = 0x03;
	currEntry->firstCluster = d->m_meta->rootDir;
	currEntry->size = d->m_meta->rootSize;

	// ime entry-ja koje trazimo u ulazima
	while (name = pi->next())
	{
		Entry* ch = searchEntry(d, currEntry, name);
		delete currEntry;
		currEntry = ch;
		if (currEntry == 0)
			break;
	}

	delete pi;

	return currEntry;
}

/*
pokusava da nadje odredjeni fajl/folder unutar Entry-ja
1. dohvata redom sve klastere koji pripadaju folderu
1.1. iterira kroz sve ulaze unutar klaster i poredi imena u pokusaju da nadje naziv
*/
Entry* KernelFS::searchEntry(Disk* d, Entry* parent, char* name)
{
	// ako je fajl, nema sta da trazimo
	if (parent->attributes = 0x01)
		return 0;

	// prazan folder
	if (parent->size == 0)
		return 0;

	Entry* res = 0;

	// dohvatamo koliko klastera ima ukupno parent folder
	int clusterCnt = (parent->size - 1) / 100 + 1;
	ClusterNo toReadFrom = parent->firstCluster;
	ClusterNo nextCluster = d->m_FAT[toReadFrom];
	int leftEntriesToTest = parent->size;

	bool found = false;
	char* data = new char[2048];
	Entry* entries;

	// za svaki klaster koji pripada
	while (!toReadFrom == 0 && !found)
	{
		d->readCluster(toReadFrom, data);
		entries = (Entry*)data;

		for (int i = 0; i < 100 && leftEntriesToTest>0 && !found; i++, leftEntriesToTest--)
		{
			Entry e = entries[i];
			// nasli smo :)
			if (matchName(e, name))
			{
				res = copyEntry(e);
				found = true;
			}
		}

		toReadFrom = nextCluster;
		nextCluster = d->m_FAT[toReadFrom];
	}

	delete[] data;
	return res;
}

/*
1. nalazimo pocetak i kraj Entry-ja u FATu
2. pocetak je nov pocetak liste slobodnih, kraj se nadovezuje sa ostatkom
*/
void KernelFS::deleteEntry(Disk* d, Entry* e)
{
	// idi sve do poslednjeg klastera
	ClusterNo klid = e->firstCluster;
	while (d->m_FAT[klid]!=0)
		klid = d->m_FAT[klid];

	// poslednji klaster nadovezuje ostatak free liste
	d->m_FAT[klid] = d->m_meta->freeNode;

	// pocetak liste slobodnih je sada ono sto je bio prvi klaster podatka
	d->m_meta->freeNode = e->firstCluster;
}