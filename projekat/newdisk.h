#ifndef _newdisk_h_
#define _newdisk_h_

#include <Windows.h>
#include "part.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "cache.h"
#include "fs.h"
#include "pathparser.h"
#include "filemt.h"
// mt
#include <vector>
#include <queue>
#include <map>
#include <string>

#define SOCL sizeof(ClusterNo)

//#define USE_CACHE

struct Disk
{
    // partition
    Partition* partition;

    // meta data
    struct _meta
    {
        ClusterNo
            freeNode,
            fatSize,
            rootDir,
            rootSize;
    } meta;

    // cached FAT
    ClusterNo* FAT;

    // cache
    CacheLRU cache;

    // mt
    std::map<std::string, std::queue<HANDLE>> filetable;
	HANDLE unmountMutex;
	bool unmountRequest;
	HANDLE formatMutex;
	bool formatRequest;

    Disk(Partition* _p)
        : cache(CACHE_SIZE, *this)
        , unmountMutex(CreateMutex(NULL, FALSE, NULL))
        , unmountRequest(false)
		, formatMutex(CreateMutex(NULL, FALSE, NULL))
		, formatRequest(false)
    {
        char w_buffer[2048];
        ClusterNo* buffer = (ClusterNo*)w_buffer;

        // reading meta data
        _p->readCluster(0, w_buffer);
        meta.freeNode   = buffer[0];
        meta.fatSize    = buffer[1];
        meta.rootDir    = buffer[2];
        meta.rootSize   = buffer[3];

        // caching entire FAT table
        FAT = new ClusterNo[meta.fatSize];
		ClusterNo FATClsCnt = (meta.fatSize + 511) / 512;
		ClusterNo left = meta.fatSize;
		for (ClusterNo i = 0; i < FATClsCnt; i++)
        {
            _p->readCluster(1+i, w_buffer);
            memcpy(FAT + (meta.fatSize - left), w_buffer, SOCL*(left<512 ? left : 512));
            left -= 512;
        }

        // caching some data
        // TODO

        // assign partition
        partition = _p;
    }

    ~Disk();
};

// cluster-wise
int readCluster(Disk& _d, ClusterNo _id, char* _buffer);
int writeCluster(Disk& _d, ClusterNo _id, const char* _buffer);
ClusterNo allocate(Disk& _d);
ClusterNo offset(Disk& _d);

// managment-wise
bool createEntry(Disk& _d, char* _fname, Entry* _e=0);      // uradjeno
bool getEntry(Disk& _d, Entry& _e, char* _fname);           // uradjeno
bool deleteEntry(Disk& _d, char* _path);                    // uradjeno
void listDir(Disk& _d, Entry& _dir, Entry * _entries);     // uradjeno

bool isOpen(Disk& _d, const char* _fname);
void waitFile(Disk& _d, std::string& _file, HANDLE mutex);
bool closeFile(Disk& _d, KernelFile* _file);

// debug
void tree(Disk& _d, bool info=true);
bool matchName(Entry& e, char* name);


#endif