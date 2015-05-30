#include "cache.h"

char* read(CacheLRU& _cache, ClusterNo _id)
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
            return temp->buffer;
        }
        temp = temp->next;
        i++;
    }
    return 0;
}

void write(CacheLRU& _cache, ClusterNo _id, char* _buffer)
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
            break;
        }
        temp = temp->next;
        i++;
    }
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