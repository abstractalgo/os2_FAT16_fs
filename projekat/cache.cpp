#include "cache.h"

bool readCache(CacheLRU& _cache, ClusterNo _id, char* _buffer)
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
            memcpy(_buffer, temp->buffer, 2048);
            return true;
        }
        temp = temp->next;
        i++;
    }
    return false;
}

void writeCache(CacheLRU& _cache, ClusterNo _id, const char* _buffer)
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
            return;
        }
        temp = temp->next;
        i++;
    }

    // not in the cache
    temp = _cache.root->prev;
    memcpy(temp->buffer, _buffer, BUFF_SIZE);
    temp->id = _id;
    _cache.root = temp;
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