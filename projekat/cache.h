#ifndef _cache_h_
#define _cache_h_

#include "part.h"
#include "kernel_fs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define CACHE_SIZE 5

struct CacheRecord
{
    ClusterNo id;
    char* buffer;
    CacheRecord *prev, *next;

    CacheRecord()
        : prev(0)
        , next(0)
        , buffer(new char[2048])
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

    CacheLRU(uint8_t _size)
    {
        root = 0;
        for (uint8_t i = 0; i < _size; i++)
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
        CacheRecord* ncr = new CacheRecord;
    }
};

char* read(CacheLRU& _cache, ClusterNo _id)
{
    CacheRecord* temp = _cache.root;
    while (temp!=0)
    {
        if (temp->id == _id)
        {
            if (_cache.root != temp)
            {
                // odvezi
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                // privezi
                _cache.root->prev->next = temp;
                temp->prev = _cache.root->prev;
                _cache.root->prev = temp;
                temp->next = _cache.root;
                // root
                _cache.root = temp;
            }
            return temp->buffer;
        }
        temp = temp->next;
    }
    return 0;
}

void write(CacheLRU& _cache, ClusterNo _id, char* _buffer)
{
    CacheRecord* temp = _cache.root;
    while (temp != 0)
    {
        if (temp->id == _id)
        {
            memcpy(temp->buffer, _buffer, 2048);
            if (_cache.root != temp)
            {
                // odvezi
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                // privezi
                _cache.root->prev->next = temp;
                temp->prev = _cache.root->prev;
                _cache.root->prev = temp;
                temp->next = _cache.root;
                // root
                _cache.root = temp;
            }
        }
        temp = temp->next;
    }
}

#endif