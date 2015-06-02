#include "newdisk.h"

int readCluster(Disk& _d, ClusterNo _id, char* _buffer)
{
#ifdef USE_CACHE
    if (readCache(_d.cache, _id, _buffer))
        return 1;
#endif
    int t = _d.partition->readCluster(offset(_d)+_id, _buffer);
#ifdef USE_CACHE
    writeCache(_d.cache, _id, _buffer);
#endif
    return t;
}

int writeCluster(Disk& _d, ClusterNo _id, const char* _buffer)
{
#ifdef USE_CACHE
    writeCache(_d.cache, _id, _buffer);
#endif
    return _d.partition->writeCluster(offset(_d)+_id, _buffer);
}

ClusterNo offset(Disk& _d)
{
    return (_d.meta.fatSize + 511) / 512 + 1;
}

ClusterNo allocate(Disk& _d)
{
    ClusterNo freeNode = _d.meta.freeNode;
    _d.meta.freeNode = _d.FAT[_d.meta.freeNode];
    return /*offset(_d) +*/ freeNode;
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

void listDir(Disk& _d, Entry& _dir, Entry *& _entries)
{
    char w_buffer[2048];
    Entry* e_buffer = (Entry*)w_buffer;

    ClusterNo brojUlaza = _dir.size;
    _entries = new Entry[brojUlaza];

    ClusterNo brojKlastera = (_dir.size + 101) / 102;
    ClusterNo preostalo = brojUlaza;
    ClusterNo cid = _dir.firstCluster;
    for (uint8_t i = 0; i < brojKlastera; i++)
    {
        readCluster(_d, cid, w_buffer);
        uint8_t limit = preostalo < 102 ? preostalo : 102;
        for (uint8_t eid = 0; eid < limit; eid++)
        {
            _entries[i * 102 + eid] = e_buffer[eid];
        }
        cid = _d.FAT[cid];
    }
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
            _d.FAT[cid] = _d.meta.freeNode;
            _d.meta.freeNode = cid;
            cid = _d.FAT[cid];
        }

        // ukloni iz naddirektorijuma TODO
        ClusterNo totalClusters;
        if (parent_folder.size-idx < 102)
        {
        }
    }
    return false;
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
    }
    
    printf(" %s%c%s\n", _e.name, _e.attributes == 0x01 ? '.' : '\0', _e.attributes == 0x01 ? _e.ext : "");
    if (info)
    {
        printf(" [%s, %d, %d]", _e.attributes == 0x01 ? "f" : (_e.attributes == 0x02 ? "d" : (_e.attributes == 0x03 ? "r" : "u")), _e.firstCluster, _e.size);
    }   

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
    for (uint8_t i = 0; i < 8; dir.name[i++] = '\0');
    for (uint8_t i = 0; i < 3; dir.ext[i++] = '\0');

    write(_d, dir, 0, info);
}