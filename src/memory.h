#pragma once

#include <stdlib.h>
#include <string.h>

#include "defines.h"

#ifndef offsetof
#define offsetof(s, m) ((size_t)&(((s*)NULL)->m))
#endif

static inline void* checkedMalloc(size_t size)
{
    void* tmp = malloc(size);
    
    if (!tmp)
        exit(BAD_ALLOC);

    return tmp;
}

static inline void* checkedRealloc(void* ptr, size_t newSize)
{
    void* tmp = realloc(ptr, newSize);    
    
    if (!tmp)
        exit(BAD_ALLOC);

    return tmp;
}

static inline void eraseFromArr(void* arr, size_t index, size_t arrElemCount, size_t elemSize)
{
    char* bytePtr = (char*)arr;

    size_t elemsToMove = arrElemCount - index - 1;
    void* dst = &bytePtr[index * elemSize];
    void* src = &bytePtr[(index + 1) * elemSize];
    size_t dataSize = elemSize * elemsToMove;

    memmove(dst, src, dataSize);
}
