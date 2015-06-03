#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define AA_KB <<10
#define AA_MB <<20
#define AA_GB <<30

#define AA_SIZE 20ul<<20

namespace aa
{
    struct _mem_manager
    {
        void* memory;
    } mem_manager;

    void init();
    void* allocate(size_t _cnt);
    void* realloc(void* _pointer, size_t _cnt);
    void free(void* _pointer);
}