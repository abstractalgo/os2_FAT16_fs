#ifndef _os2_disk_h_
#define _os2_disk_h_

#include "part.h"
#include <stdio.h>

//#define USE_CACHE

#ifdef USE_CACHE

#define CACHE_SIZE 5

////////////////////////////////////////////////////////////////////////////////
// Class name: Cache
////////////////////////////////////////////////////////////////////////////////
class Cache
{
public:
    Cache(int size=0) {}
    virtual ClusterNo* getData(ClusterNo id) = 0;
    virtual void writeData(ClusterNo id, const char* buffer) = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: LinkedCache
////////////////////////////////////////////////////////////////////////////////
class LinkedCache : public Cache
{
private:
    struct CacheEntry
    {
        bool        m_dirty;
        ClusterNo   m_id;
        ClusterNo*  m_data;
        CacheEntry
            *m_prev,
            *m_next;

        CacheEntry(ClusterNo id)
            : m_id(id)
            , m_dirty(false)
            , m_data(new ClusterNo[512])
            , m_prev(0)
            , m_next(0)
        {
        }
    };

    int m_size;
    int m_count;
    CacheEntry* m_first;
public:
    LinkedCache(int size)
        : m_size(size)
        , m_count(0)
        , m_first(0)
    {
    }

    ClusterNo* getData(ClusterNo id)
    {
        CacheEntry* t = m_first;
        while(t!=0)
        {
            if(t->m_id == id)                                                   // postoji u kesu
            {
                if(t==m_first)                                                  // prvi u listi
                    return m_first->m_data;
                if(t->m_next!=0)                                                // u sredini liste
                {
                    t->m_prev->m_next = t->m_next;
                    t->m_next->m_prev = t->m_prev;
                }
                else                                                            // na kraju lsite
                {
                    t->m_prev->m_next = 0;
                }
                t->m_next = m_first;
                t->m_prev = 0;
                m_first->m_prev = t;
                m_first = t;
                return m_first->m_data;
            }
            t = t->m_next;
        }
        return 0;
    }

    void writeData(ClusterNo id, const char* buffer)
    {
        CacheEntry* t = m_first;
        CacheEntry* old = 0;
        while(t!=0)
        {
            if(t->m_id == id)
            {
                // upisi podatke, gurni na pocetak. dirty
                memcpy(t->m_data, buffer, 2048);
                t->m_dirty = true;
                if(t==m_first)
                    return;
                t->m_prev->m_next = t->m_next;
                if(t->m_next!=0)
                    t->m_next->m_prev = t->m_prev;
                m_first->m_prev = t;
                t->m_next = m_first;
                t->m_prev = 0;
                return;
            }
            old = t;
            t = t->m_next;
        }
        // ima ih manje -> napravi novi
        if(m_count<m_size)
        {
            t = new CacheEntry(id);
            memcpy(t->m_data, (ClusterNo*)buffer, 512);
            t->m_dirty = true;
            old->m_next = t;
            t->m_prev = old;
            m_count++;
        }
        // pun kes -> proveri lru da li je dirty, ako jeste upisi ga, onda ga prepisi
        else
        {
            if(old->m_dirty)
            {

            }
        }
    }
};
#endif

////////////////////////////////////////////////////////////////////////////////
// Class name: Disk
////////////////////////////////////////////////////////////////////////////////
// Disk se ponasa kao wrapper za klasu Partition. Ima iste funkcionalnosti
// kao Partition s tom razlikom sto ume da kesira podatke.
////////////////////////////////////////////////////////////////////////////////
class Disk
{
    Partition* m_partition;

    // deskriptor
    struct desc
    {
        ClusterNo freeNode;
        ClusterNo fatSize;
        ClusterNo rootDir;
        ClusterNo rootSize;

        desc()
            : freeNode(0)
            , fatSize(0)
            , rootDir(0)
            , rootSize(0)
        {}
    } *m_meta;

    // kesirani podaci
    ClusterNo *m_FAT;
    #ifdef USE_CACHE
    Cache* m_cache;
    #endif

	friend class KernelFS;

public:
    Disk(Partition* p)
        : m_partition(p)
    {
        #ifdef USE_CACHE
        // m_cache = new LinkedCache(CACHE_SIZE);
        #endif
        
        char* w_buffer = new char[2048];
        ClusterNo* buffer = (ClusterNo*)w_buffer;

        // citanje deskriptora
        m_meta = new desc;
        p->readCluster(0, w_buffer);
        m_meta->freeNode  = buffer[0];    // read free node
        m_meta->fatSize   = buffer[1];    // read FAT size
        m_meta->rootDir   = buffer[2];    // read root start
        m_meta->rootSize  = buffer[3];    // read root size

        // kesiranje FAT tabele
        ClusterNo FATClsCnt = (m_meta->fatSize + 511) / 512;
        buffer              = new ClusterNo[FATClsCnt*512];
        w_buffer            = (char*)buffer;
        m_FAT               = new ClusterNo[m_meta->fatSize];
        
        /*for (ClusterNo ci = 0; ci < FATClsCnt; ci++)
            p->readCluster(1 + ci, w_buffer + 2048 * ci);
        for (ClusterNo idx = 0; idx < m_meta->fatSize; idx++)
            m_FAT[idx] = buffer[idx];*/

        delete[] w_buffer;
    }

    ~Disk()
    {
        #ifdef USE_CACHE
        // delete m_cache;
        // m_cache = 0;
        #endif
        delete[] m_FAT;
        delete m_meta;
    }

    int readCluster(ClusterNo id, char* buffer)
    {
#ifdef USE_CACHE
        // ClusterNo* data = m_cache->getData(id);
        // if(data!=0)
        // {
        //     memcpy(buffer, data, 2048);
        //     return 1;
        // }
        // // upis u kes i onda citanje iz kesa?
        // m_partition->readCluster(id, buffer);
        // return 1;
#endif

        return m_partition->readCluster(id, buffer);
    }

    int writeCluster(ClusterNo id, const char* buffer)
    {
#ifdef USE_CACHE
        // m_cache->writeData(id, buffer);
        // // upis odmah na disk?
        // return 0;   
#endif

        return m_partition->writeCluster(id, buffer);
    }

    int format()
    {
        // racunanje velicine
        ClusterNo p_size = m_partition->getNumOfClusters();
        ClusterNo dataClsCnt = ((p_size - 1) * 512) / 513;
        ClusterNo N = p_size - dataClsCnt;

        // alokacija za klastere
        ClusterNo* buffer = new ClusterNo[512];
        char* w_buffer = (char*)buffer;

        // upis deskriptora
        buffer[0] = 1;          // pocetak free liste
        buffer[1] = dataClsCnt; // FAT size
        buffer[2] = 0;          // pocetak root dir
        buffer[3] = 0;          // root size
        m_partition->writeCluster(0, w_buffer);
		printf("Data size: %d\nData start: %d\n", dataClsCnt, N);
		delete[] buffer;

        // formatiranje FAT tabele
        for (ClusterNo cnt = 0; cnt < dataClsCnt; cnt++)
			m_FAT[cnt] = cnt == dataClsCnt-1 ? 0 : cnt + 1;			// TODO fat je u memoriji, jos uvek nije na disku
		m_FAT[0]=0;
		
        // clean-up
        return 1;
    }
};

#endif