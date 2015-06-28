#include "cache.h"
#include "disk.h"

char readCache(CacheLRU& _cache, ClusterNo _id, char* _buffer)
{
    CacheRecord* temp = _cache.root;
    uint8_t i = 0;
    while (i<_cache.size)
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
            memcpy(_buffer, temp->buffer, BUFF_SIZE);
			temp->dirty = false;
            return 1;
        }
        temp = temp->next;
        i++;
    }
    return 0;
}

char writeCache(CacheLRU& _cache, ClusterNo _id, const char* _buffer)
{
    CacheRecord* temp = _cache.root;
    uint8_t i = 0;
    while (i<_cache.size)
    {
        if (temp->id == _id || temp->id==-1)
        {
            // fill with data
            memcpy(temp->buffer, _buffer, BUFF_SIZE);
            temp->id = _id;
			temp->dirty = true;

            // move
            if (_cache.root != temp)
            {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                _cache.root->prev->next = temp;
                temp->prev = _cache.root->prev;
                _cache.root->prev = temp;
                temp->next = _cache.root;
                _cache.root = temp;
            }
            return 1;
        }
        temp = temp->next;
        i++;
    }

    // not in the cache
    temp = _cache.root->prev;					// dohvati poslednjeg
	char r = 0;
	if (temp->dirty)							// treba ga upisati na disk
		r = _cache.d.partition->writeCluster(offset(_cache.d)+_id, _buffer);
    memcpy(temp->buffer, _buffer, BUFF_SIZE);
    temp->id = _id;
    _cache.root = temp;
	return r;
}

void debug_write(CacheLRU& _cache)
{
    printf("Cache size: %d\n", _cache.size);
    CacheRecord *temp = _cache.root;
    for (uint8_t i = 0; i < _cache.size; i++)
    {
        printf("[%d]: id(%d)\tbuffer(%c%c...)\n", i, temp->id, temp->buffer[0], temp->buffer[1]);
        temp = temp->next;
    }
}

CacheLRU::CacheLRU(uint8_t _size, Disk& _d)
	: size(_size)
	, d(_d)
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

CacheLRU::~CacheLRU()
{
	CacheRecord* temp = root;
	CacheRecord* old = temp;
	for (uint8_t i = 0; i < size; i++)
	{
		old = temp;
		temp = temp->next;
		if (old->dirty)
			d.partition->writeCluster(offset(d) + old->id, old->buffer);
		delete old;
	}
}