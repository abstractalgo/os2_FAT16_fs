#ifndef _pathparser_h_
#define _pathparser_h_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct PathParser
{
    // attributes
    char disk;
    uint8_t partsNum;
    char** parts;

    //methods
    PathParser()
        : parts(0)
        , disk('\0')
        , partsNum(0)
    {}

    ~PathParser()
    {
        for (uint8_t i = 0; i < partsNum; i++)
            delete[] parts[i];
        delete[] parts;
        partsNum = 0;
    }
};

#define SOC sizeof(char)
#define SOCP sizeof(char*)
bool parse(PathParser& _p, const char* _path, bool _zt = true)
{
    uint16_t l      = strlen(_path);
    if (l < 3) return false;

    _p.disk         = _path[0];
    _p.partsNum     = 0;
    uint16_t i      = 3;

    // parse
    while (i<l)
    {
        uint16_t old_i = i;
        // get to the end of the string or the separator
        while (i < l && _path[i] != '\\') i++;
        
        // allocate memory and copy data
        _p.parts = (char**)realloc(_p.parts, (_p.partsNum + 1) * SOCP);
        _p.parts[_p.partsNum] = (char*)malloc(SOC*(i - old_i + _zt ? 1 : 0));
        memcpy(_p.parts[_p.partsNum], _path + old_i, SOC*(i - old_i));
        if (_zt) _p.parts[_p.partsNum][i - old_i] = '\0';

        _p.partsNum++;
        i++;
    }

    return true;
}

char* getAt(PathParser& _p, uint8_t _index)
{
    return _index >= _p.partsNum
                ? 0
                : _p.parts[_index];
}

void write(PathParser& _p)
{
    printf("Disk: %c\n", _p.disk);
    printf("Delova: %d\n", _p.partsNum);
    for (uint8_t i = 0; i < _p.partsNum; i++)
        printf("\ [%d]: %s\n", i, get(_p, i));
}

#endif