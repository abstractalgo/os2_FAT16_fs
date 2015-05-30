#ifndef _newdisk_h_
#define _newdisk_h_

#include "part.h"
#include "kernel_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "cache.h"

#define SOCL sizeof(ClusterNo)

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

    Disk(Partition* _p)
        : cache(CACHE_SIZE)
    {
        char w_buffer[2048];
        ClusterNo* buffer = (ClusterNo*)w_buffer;

        // reading meta data
        _p->readCluster(0, w_buffer);
        meta.freeNode   = buffer[0];
        meta.fatSize    = buffer[1];
        meta.rootDir    = buffer[2];
        meta.rootSize   = buffer[3];

        // storing entire FAT table
        FAT = new ClusterNo[meta.fatSize];
        uint16_t FATClsCnt = (meta.fatSize + 511) / 512;
        uint16_t left = meta.fatSize;
        for (uint16_t i = 0; i < FATClsCnt; i++)
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

    ~Disk()
    {
        delete[] FAT;
    }
};

int format(Disk& d);
int readCluster(Disk& _d, ClusterNo _id, char* _buffer);
int writeCluster(Disk& _d, ClusterNo _id, const char* _buffer);
bool getEntry(Disk& _d, Entry& _e);

#endif