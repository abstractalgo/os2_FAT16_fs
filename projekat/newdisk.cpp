#include "newdisk.h"
#include "kernel_file.h"

int readCluster(Disk& _d, ClusterNo _id, char* _buffer)
{
#ifdef USE_CACHE
    // procitaj iz kesa, ako je u kesu
    if (readCache(_d.cache, _id, _buffer))
        return 1;

    // ako nije u kesu, ucitaj u kes i vrati podatak iz kesa
    int t = _d.partition->readCluster(offset(_d)+_id, _buffer);
    writeCache(_d.cache, _id, _buffer);
    return t;
#else
    return _d.partition->readCluster(offset(_d) + _id, _buffer);
#endif
}

int writeCluster(Disk& _d, ClusterNo _id, const char* _buffer)
{
#ifdef USE_CACHE
    return writeCache(_d.cache, _id, _buffer);
#else
    return _d.partition->writeCluster(offset(_d)+_id, _buffer);
#endif
}

ClusterNo offset(Disk& _d)
{
    return (_d.meta.fatSize + 511) / 512 + 1;
}

ClusterNo allocate(Disk& _d)
{
    ClusterNo freeNode = _d.meta.freeNode;
    _d.meta.freeNode = _d.FAT[_d.meta.freeNode];
    _d.FAT[freeNode] = 0;
    return /*offset(_d) +*/ freeNode;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool createEntry(Disk& _d, char* _fname, Entry* _e)
{
    PathParser ppath;
    parse(ppath, _fname);

    Entry parent_folder;

    // nadji folder gde treba napraviti novi entry
    if (getEntry(_d, parent_folder, combine(ppath, ppath.partsNum - 1)))
    {
        // napravi novi entry
        Entry ent;
        ent.firstCluster = allocate(_d);
        _d.FAT[ent.firstCluster] = 0;
        for (uint8_t i = 0; i < 8; ent.name[i++] = '\0');
        for (uint8_t i = 0; i < 3; ent.ext[i++] = '\0');
        ent.size = 0;
        
        char* name = ppath.parts[ppath.partsNum - 1];
        if (isFolder(name))
        {
            ent.attributes = 0x02;
            memcpy(ent.name, name, strlen(name)*SOC);
        }
        else
        {
            ent.attributes = 0x01;
            uint8_t i = 0;
            while (name[i] != '.') i++;
            memcpy(ent.name, name, i*SOC);
            memcpy(ent.ext, name + i + 1, (strlen(name) - i - 1)*SOC);
        }

        // nadji ili alociraj klaster sa dovoljno mesta
        ClusterNo cid = parent_folder.firstCluster;
        while (_d.FAT[cid] != 0) cid = _d.FAT[cid];
        if (parent_folder.size > 0 && parent_folder.size % 102 == 0)
        {
            ClusterNo na = allocate(_d);
            _d.FAT[cid] = na;
            _d.FAT[na] = 0;
            cid = na;
        }

        // upis novonapravljenog entry-ja u klaster
        {
            char* w_buffer = new char[2048];
            Entry* entries = (Entry*)w_buffer;
            readCluster(_d, cid, w_buffer);
            entries[(parent_folder.size) % 102] = ent;
            writeCluster(_d, cid, w_buffer);
            delete[] w_buffer;
        }

        if (_e)
            *_e = ent;

        // apdejtuj velicinu nadfoldera (jer je u njemu zapisana velicina parent_foldera)
        if (parent_folder.attributes == 0x03)
        {
            _d.meta.rootSize++;
        }
        else
        {
            // nadji nad folder, za promeni velicine
            Entry _dir;
            getEntry(_d, _dir, combine(ppath, ppath.partsNum - 2));
            char w_buffer[2048];
            Entry* e_buffer = (Entry*)w_buffer;

            ClusterNo brojUlaza = _dir.size;

            ClusterNo brojKlastera = (_dir.size + 101) / 102;
            ClusterNo preostalo = brojUlaza;
            ClusterNo cid = _dir.firstCluster;
            for (ClusterNo i = 0; i < brojKlastera; i++)
            {
                readCluster(_d, cid, w_buffer);
                e_buffer = (Entry*)w_buffer;
                ClusterNo limit = preostalo < 102 ? preostalo : 102;
                bool fnd = false;
                for (ClusterNo eid = 0; eid < limit; eid++)
                {
                    if (e_buffer[eid].firstCluster == parent_folder.firstCluster)
                    {
                        fnd = true;
                        e_buffer[eid].size++;
                        writeCluster(_d, cid, w_buffer);
                        break;
                    }
                }
                if (fnd) break;
                cid = _d.FAT[cid];
            }
        }
    }
    return false;
}

bool getEntry(Disk& _d, Entry& _e, char* _fname)
{
    // parsiraj putanju
    PathParser ppath;
    parse(ppath, _fname);

    // root entry
    Entry dir;
    dir.attributes = 0x03;
    dir.firstCluster = _d.meta.rootDir;
    dir.size = _d.meta.rootSize;

    // root dir
    if (!ppath.partsNum)
    {
        _e = dir;
        return true;
    }

    bool fnd = false;
    // za svaki deo imena
    for (uint8_t i = 0; i < ppath.partsNum; i++)
    {
        if (dir.size == 0 || (i == ppath.partsNum - 1 && dir.attributes == 0x01))
            return false;

        // uzmi sve entry-je foldera
        Entry* entries = new Entry[dir.size];
        listDir(_d, dir, entries);
        fnd = false;
        // pronadji entry u listi
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
        if (!fnd) return false;
    }

    if (fnd)
    {
        _e = dir;
        return true;
    }

    return false;
}

bool deleteEntry(Disk& _d, char* _path)
{
    PathParser ppath;
    parse(ppath, _path);

    Entry parent_folder;
    Entry ent;

    // nadji parent folder
    if (getEntry(_d, parent_folder, combine(ppath, ppath.partsNum - 1)))
    {
        Entry* entries = new Entry[parent_folder.size];
        listDir(_d, parent_folder, entries);

        // nadji entry unutar parent foldera
        bool fnd = false;
        uint16_t idx = 0;

        for (uint16_t i = 0; i < parent_folder.size; i++)
        {
            if (matchName(entries[i], ppath.parts[ppath.partsNum - 1]))
            {
                // nasao entry
                ent = entries[i];
                fnd = true;
                idx = i;
                break;
            }
        }
        delete[] entries;

        // ako nije nadjen, fuu~
        if (!fnd)
            return false;

        // ako je neprazan folder, fuu~
        if (ent.attributes == 0x02 && ent.size != 0)
            return false;

        // oslobodi klastere koji pripadaju entry-ju
        ClusterNo cid = ent.firstCluster;
        while (cid != 0)
        {
            ClusterNo next = _d.FAT[cid];
            _d.FAT[cid] = _d.meta.freeNode;
            _d.meta.freeNode = cid;
            cid = next;
        }

        // ukloni iz parent foldera
        Entry last_entry;
        ClusterNo last_cid = parent_folder.firstCluster, ent_cid = last_cid;
        while (_d.FAT[last_cid] != 0)
        {
            last_cid = _d.FAT[last_cid];
            if (idx < 103) ent_cid = last_cid;
            idx -= 102;
        }
        char w_buffer[2048];
        Entry* e_buffer = (Entry*)w_buffer;
        readCluster(_d, last_cid, w_buffer);
        last_entry = e_buffer[(parent_folder.size+102) % 102-1];
        readCluster(_d, ent_cid, w_buffer);
        e_buffer[idx] = last_entry;
        writeCluster(_d, ent_cid, w_buffer);

        // smanji parent folder ako treba sto se broja klastera tice
        if (parent_folder.size - 1 > 0 && (parent_folder.size + 102) % 102 == 0)
        {
            ClusterNo cl = parent_folder.firstCluster;
            while (_d.FAT[_d.FAT[cl]] != 0) cl = _d.FAT[cl];
            _d.FAT[cl] = 0;
            _d.FAT[last_cid] = _d.meta.freeNode;
            _d.meta.freeNode = last_cid;
        }
        
        // smanji velicinu u nadfolderu
        if (parent_folder.attributes == 0x03)
        {
            _d.meta.rootSize--;
        }
        else
        {
            // nadji nad folder, za promeni velicine
            Entry _dir;
            getEntry(_d, _dir, combine(ppath, ppath.partsNum - 2));
            char w_buffer[2048];
            Entry* e_buffer = (Entry*)w_buffer;

            ClusterNo brojUlaza = _dir.size;

            ClusterNo brojKlastera = (_dir.size + 101) / 102;
            ClusterNo preostalo = brojUlaza;
            ClusterNo cid = _dir.firstCluster;
            for (ClusterNo i = 0; i < brojKlastera; i++)
            {
                readCluster(_d, cid, w_buffer);
                e_buffer = (Entry*)w_buffer;
                ClusterNo limit = preostalo < 102 ? preostalo : 102;
                bool fnd = false;
                for (ClusterNo eid = 0; eid < limit; eid++)
                {
                    if (e_buffer[eid].firstCluster == parent_folder.firstCluster)
                    {
                        fnd = true;
                        e_buffer[eid].size--;
                        writeCluster(_d, cid, w_buffer);
                        break;
                    }
                }
                if (fnd) break;
                cid = _d.FAT[cid];
            }
        }
    }
    return true;
}

void listDir(Disk& _d, Entry& _dir, Entry * _entries)
{
    char w_buffer[2048];
    Entry* e_buffer = (Entry*)w_buffer;

    ClusterNo brojUlaza = _dir.size;
    //_entries = new Entry[brojUlaza];

    ClusterNo brojKlastera = (_dir.size + 101) / 102;
    ClusterNo preostalo = brojUlaza;
    ClusterNo cid = _dir.firstCluster;
    for (ClusterNo i = 0; i < brojKlastera; i++)
    {
        readCluster(_d, cid, w_buffer);
        ClusterNo limit = preostalo < 102 ? preostalo : 102;
        for (ClusterNo eid = 0; eid < limit; eid++)
        {
            _entries[i * 102 + eid] = e_buffer[eid];
        }
        cid = _d.FAT[cid];
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

static void write(Disk& _d, Entry& _e, uint8_t _level, bool info)
{
    if (_level)
    {
        for (uint8_t i = 0; i < _level; i++) printf("  ");
        printf("#--");
        printf(" %.8s%c%.3s", _e.name, _e.attributes == 0x01 ? '.' : '\0', _e.attributes == 0x01 ? _e.ext : "");
    }
    
    if (info)
    {
        printf(" [%s, %d, %d]", _e.attributes == 0x01 ? "f" : (_e.attributes == 0x02 ? "d" : (_e.attributes == 0x03 ? "r" : "u")), _e.firstCluster, _e.size);
    }   
    putchar('\n');

    // ako je folder, nastavi ispis rekurzivno
    if (_e.attributes > 1)
    {
        Entry* slotovi = new Entry[_e.size];
        listDir(_d, _e, slotovi);
        for (uint8_t i = 0; i < _e.size; i++)
        {
            write(_d, slotovi[i], _level + 1, info);
        }
        delete[] slotovi;
    }
}

void tree(Disk& _d, bool info)
{
    Entry dir;
    dir.attributes = 0x03;
    dir.firstCluster = _d.meta.rootDir;
    dir.size = _d.meta.rootSize;

    write(_d, dir, 0, info);
}

bool matchName(Entry& e, char* name)
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
        if ((tacka && e.ext[j] != name[i]) ||
            (!tacka && e.name[j] != name[i]))
            return false;
        j++;
        i++;
    }
    return true;
}

Disk::~Disk()
{
    char w_buffer[2048];
    ClusterNo* buffer = (ClusterNo*)w_buffer;

    // upis meta podataka
    buffer[0] = meta.freeNode;          // free node
    buffer[1] = meta.fatSize;           // FAT size
    buffer[2] = meta.rootDir;           // root dir
    buffer[3] = meta.rootSize;          // root size
    partition->writeCluster(0, w_buffer);

    // upis FAT tabele
    ClusterNo ccnt = (meta.fatSize + 511) / 512;
    ClusterNo left = meta.fatSize;
    for (ClusterNo i = 0; i < ccnt; i++)
    {
        ClusterNo cpycnt = ((left > 512) ? 512 : left);
        memcpy(buffer, FAT+(meta.fatSize-left), cpycnt*sizeof(ClusterNo));
        partition->writeCluster(1 + i, w_buffer);
        left -= cpycnt;
    }

    // upis klastera sa podacima
    //for (ClusterNo i = 0; i < meta.fatSize; i++)
    //{
    //    readCluster(*this, i, w_buffer);
    //    partition->writeCluster(1 + ccnt + i, w_buffer);
    //}

    CloseHandle(unmountMutex);
    CloseHandle(formatMutex);

    delete[] FAT;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

bool isOpen(Disk& _d, const char* _fname)
{
    std::string name(_fname);
    return (_d.filetable.find(name) != _d.filetable.end());
}

void waitFile(Disk& _d, std::string& fname, HANDLE mutex)
{
    // proveri da li je otvoren fajl
    HANDLE s = CreateSemaphore(NULL, 0, 32, NULL);
    _d.filetable[fname].push(s);
    //ReleaseMutex(mutex);
    //WaitForSingleObject(s, INFINITE);
    SignalObjectAndWait(mutex, s, INFINITE, FALSE);
    WaitForSingleObject(mutex, INFINITE);
    CloseHandle(s);
}

bool closeFile(Disk& _d, KernelFile* _file)
{
    auto it = _d.filetable.find(_file->path);
    if (it != _d.filetable.end())
    {
        if (it->second.size() > 0)
        {
            HANDLE sem = it->second.front();
            it->second.pop();
            ReleaseSemaphore(sem, 1, NULL);
        }
        // niko ne ceka na fajl
        else
        {
            // ukloni ga iz liste otvorenih fajlova
            _d.filetable.erase(it);
        }
    }

    if (_d.filetable.size() == 0)
    {
        if (_d.formatRequest)
            ReleaseMutex(_d.formatMutex);
        else if (_d.unmountRequest)
            ReleaseMutex(_d.unmountMutex);
    }

    return true;
}