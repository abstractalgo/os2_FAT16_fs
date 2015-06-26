#ifndef _cache_h_
#define _cache_h_

#include "part.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
struct Disk;

#define CACHE_SIZE 7
#define BUFF_SIZE 2048

//struct cache_line
//{
//	ClusterNo id;
//	char* data;
//	bool dirty;
//	bool valid;
//	uint8_t next;
//
//	cache_line()
//		: next(0)
//		, dirty(false)
//		, id(~0)
//		, data(new char[BUFF_SIZE])
//		, valid(false)
//	{}
//
//	~cache_line()
//	{
//		delete[] data;
//	}
//};

struct CacheRecord
{
    ClusterNo id;
    char* buffer;
	bool dirty;
    CacheRecord *prev, *next;

    CacheRecord()
        : prev(0)
        , next(0)
        , buffer(new char[BUFF_SIZE])
        , id(-1)
		, dirty(false)
    {}

    ~CacheRecord()
    {
        delete[] buffer;
    }
};

struct CacheLRU
{
	Disk& d;
    CacheRecord* root;
    uint8_t size;

	CacheLRU(uint8_t _size, Disk& _d);
	~CacheLRU();
};

char readCache(CacheLRU& _cache, ClusterNo _id, char*);
char writeCache(CacheLRU& _cache, ClusterNo _id, const char* _buffer);
void debug_write(CacheLRU& _cache);

#endif