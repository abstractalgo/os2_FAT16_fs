#include "newdisk.h"

char format(Disk& _d)
{
    char w_buffer[2048];
    ClusterNo* buffer = (ClusterNo*)w_buffer;

    // racunanje velicine
    ClusterNo totalClsCnt = _d.partition->getNumOfClusters();
    ClusterNo dataClsCnt = ((totalClsCnt - 1) * 512) / 513;
    ClusterNo FATClsCnt = totalClsCnt - 1 - dataClsCnt;

    // upis meta podataka
    buffer[0] = 1;
    buffer[1] = dataClsCnt;
    buffer[2] = 0;
    buffer[3] = 0;
    _d.partition->writeCluster(0, w_buffer);

    // formatiranje FAT tabele
    ClusterNo left = dataClsCnt;
    for (ClusterNo cid = 0; cid < dataClsCnt; cid++)
    {
        for (ClusterNo i = 0; i < 512; i++)
        {
            buffer[i] = (cid * 512) + i;
            if (cid * 512 + i == dataClsCnt)
            {
                buffer[i] = 0;
                break;
            }
        }
        _d.partition->writeCluster(1+cid, w_buffer);
    }

    return 1;
}

char release(Disk& _d)
{
    // TODO ...
    return 0;
}

int readCluster(Disk& _d, ClusterNo _id, char* _buffer)
{
    char* data = readCache(_d.cache, _id);
    if (data)
    {
        _buffer = data;
        return 1;
    }

    int t = _d.partition->readCluster(_id, _buffer);
    writeCache(_d.cache, _id, _buffer);
    return t;
}

int writeCluster(Disk& _d, ClusterNo _id, const char* _buffer)
{
    writeCache(_d.cache, _id, _buffer);
    return _d.partition->writeCluster(_id, _buffer);
}

bool getEntry(Disk& _d, Entry& _e, char* _fname)
{
    PathParser ppath;
    parse(ppath, _fname);

    for (uint8_t i = 0; i < ppath.partsNum; i++)
    {

    }
    return true;
}

void listDir(Disk& _d, Entry& _dir, Entry *& _entries)
{
    char w_buffer[2048];
    Entry* e_buffer;

    ClusterNo brojUlaza = _dir.size;
    _entries = new Entry[brojUlaza];

    ClusterNo brojKlastera = (_dir.size + 101) / 102;
    ClusterNo preostalo = brojUlaza;
    ClusterNo cid = _dir.firstCluster;
    for (uint8_t i = 0; i < brojKlastera; i++)
    {
        readCluster(_d, cid, w_buffer);
        e_buffer = (Entry*)w_buffer;
        uint8_t limit = preostalo < 102 ? preostalo : 102;
        for (uint8_t eid = 0; eid < limit; eid++)
        {
            _entries[i * 102 + eid] = e_buffer[eid];
        }
        cid = _d.FAT[cid];
    }
}

void write(Disk& _d, Entry& _e, uint8_t _level)
{
    for (uint8_t i = 0; i<_level; i++) putchar('-');
    printf("'%.8s.%.3s' (%s, %d, %d)\n", _e.name, _e.ext, _e.attributes == 0x01 ? "fajl" : (_e.attributes == 0x02 ? "poddir" : (_e.attributes == 0x03 ? "rootdir" : "unknown")), _e.firstCluster, _e.size);

    // ako je folder, nastavi ispis rekurzivno
    if (_e.attributes > 1)
    {
        Entry* slotovi = new Entry[_e.size];
        listDir(_d, _e, slotovi);
        for (uint8_t i = 0; i < _e.size; i++)
        {
            write(_d, slotovi[i], _level + 1);
        }
        delete[] slotovi;
    }
}

void tree(Disk& _d)
{
    Entry dir;
    dir.attributes = 0x03;
    dir.firstCluster = _d.meta.rootDir;
    dir.size = _d.meta.rootSize;

    write(_d, dir, 0);
}