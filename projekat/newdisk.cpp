#include "newdisk.h"

int format(Disk& d)
{
    // TODO ...
}

int readCluster(Disk& _d, ClusterNo _id, char* _buffer)
{
    /*char* data = _d.cache[_id];
    if (data)
    {
    _buffer = data;
    return 1;
    }*/

    // cache thingy ... TODO
    return _d.partition->readCluster(_id, _buffer);
}

int writeCluster(Disk& _d, ClusterNo _id, const char* _buffer)
{
    // cache thingy ... TODO
    return _d.partition->writeCluster(_id, _buffer);
}

bool getEntry(Disk& _d, Entry& _e)
{
    // TODO ...
    return true;
}