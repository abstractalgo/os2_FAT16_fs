#include "pathparser.h"

bool parse(PathParser& _p, const char* _path, bool _zt)
{
    uint16_t l = strlen(_path);
    if (l < 3) return false;

    _p.disk = _path[0];
    _p.partsNum = 0;
    uint16_t i = 3;

    // parse
    while (i<l)
    {
        uint16_t old_i = i;
        // get to the end of the string or the separator
        while (i < l && _path[i] != '\\' && _path[i]!='\0') i++;

        // allocate memory and copy data
        _p.parts = (char**)realloc(_p.parts, (_p.partsNum + 1) * SOCP);
        _p.parts[_p.partsNum] = (char*)malloc(SOC*(i - old_i + (_zt ? 1 : 0)));
        memcpy(_p.parts[_p.partsNum], _path + old_i, SOC*(i - old_i));
        if (_zt) _p.parts[_p.partsNum][i - old_i] = '\0';

        _p.partsNum++;
        i++;
    }

    if (!isValid(_p))
    {
        return false;
    }

    return true;
}

char* getAt(PathParser& _p, uint8_t _index)
{
    return _index >= _p.partsNum
        ? 0
        : _p.parts[_index];
}

bool isFolder(char* _str)
{
    uint16_t l = strlen(_str);
    for (uint16_t i = 0; i < l; i++)
    if (_str[i] == '.')
        return false;
    return true;
}

bool isFile(char* _str)
{
    return !(isFolder(_str));
}

bool isValid(PathParser& _p)
{
    uint8_t n = _p.partsNum;
    for (uint8_t i = 0; i < n; i++)
    {
        if (isFolder(getAt(_p, i)) != (i < n - 1))
            return false;
    }
    return true;
}

void write(PathParser& _p)
{
    printf("Disk: %c\n", _p.disk);
    printf("Number of parts: %d\n", _p.partsNum);
    for (uint8_t i = 0; i < _p.partsNum; i++)
        printf("[%d]: %s\n", i, getAt(_p, i));
}

char* combine(PathParser& _p, uint8_t _n)
{
    char* res = new char[3];
    res[0] = _p.disk;
    res[1] = ':';
    res[2] = '\\';
    uint16_t size = 3;
    for (uint8_t i = 0; i < _n; i++)
    {
        uint16_t part_size = strlen(_p.parts[i]);
        size += part_size+1;
        res = (char*)realloc(res, (size+1)*SOC);
        memcpy(res+size-part_size-1, _p.parts[i], SOC*part_size);
        res[size-1] = '\\';
    }
    res = (char*)realloc(res, (size+1)*SOC);
    res[size] = '\0';
    return res;
}