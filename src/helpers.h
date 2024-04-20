#pragma once

#include <string.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(low, high, x) (max((low), min((x), (high))))
#define withinRange(low, high, x) ((x) >= (low) && (x) <= (high))

static inline void eraseFromArr(void* arr, size_t index, size_t arrElemCount, size_t elemSize)
{
    char* bytePtr = (char*)arr;

    size_t elemsToMove = arrElemCount - index - 1;
    void* dst = &bytePtr[index * elemSize];
    void* src = &bytePtr[(index + 1) * elemSize];
    size_t dataSize = elemSize * elemsToMove;

    memmove(dst, src, dataSize);
}