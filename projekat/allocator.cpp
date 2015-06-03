#include "allocator.h"

void aa::init()
{
    mem_manager.memory = malloc(AA_SIZE);
}

void* aa::allocate(size_t _cnt)
{
    return 0;
}

void* aa::realloc(void* _pointer, size_t _cnt)
{
    return 0;
}

void aa::free(void* _pointer)
{

}