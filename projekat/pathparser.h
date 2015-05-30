#ifndef _pathparser_h_
#define _pathparser_h_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SOC sizeof(char)
#define SOCP sizeof(char*)

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

bool parse(PathParser& _p, const char* _path, bool _zt = true);                 // parsira string u PathParser
char* getAt(PathParser& _p, uint8_t _index);                                    // dohvata i-to po redu clan
bool isFolder(char* _str);                                                      // provera da li je u pitanju folder
bool isFile(char* _str);                                                        // provera da li je fajl
bool isValid(PathParser& _p);                                                   // provera da li je string validan
void write(PathParser& _p);                                                     // ispis (za debug svrhe)

#endif