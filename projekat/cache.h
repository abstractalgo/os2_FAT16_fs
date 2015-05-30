#ifndef _cache_h_
#define _cache_h_

#include "part.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define CACHE_SIZE 5
#define BUFF_SIZE 2048

struct CacheRecord
{
    ClusterNo id;
    char* buffer;
    CacheRecord *prev, *next;

    CacheRecord()
        : prev(0)
        , next(0)
        , buffer(new char[BUFF_SIZE])
        , id(-1)
    {}

    ~CacheRecord()
    {
        delete[] buffer;
    }
};

struct CacheLRU
{
    CacheRecord* root;
    uint8_t size;

    CacheLRU(uint8_t _size)
        : size(_size)
    {
        root = 0;
        for (uint8_t i = 0; i < size; i++)
        {
            CacheRecord* ncr = new CacheRecord;
            if (root)
            {
                ncr->next = root;
                root->prev->next = ncr;
                ncr->prev = root->prev;
                root->prev = ncr;
            }
            else
            {
                ncr->next = ncr;
                ncr->prev = ncr;
            }
            root = ncr;
        }
    }

    ~CacheLRU()
    {
        CacheRecord *temp = root, *old;
        for (uint8_t i = 0; i < size; i++)
        {
            old = temp;
            temp = temp->next;
            delete old;
        }
    }
};

char* readCache(CacheLRU& _cache, ClusterNo _id);
void writeCache(CacheLRU& _cache, ClusterNo _id, const char* _buffer);
void debug_write(CacheLRU& _cache);

#endif