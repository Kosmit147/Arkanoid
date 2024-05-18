#pragma once

#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__) || defined(__clang__)
#define UNUSED(x) unused_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) unused_ ## x
#endif

#if defined(__GNUC__) || defined(__clang__)
#define ENSURE_PACKED   _Pragma("GCC diagnostic push")\
                        _Pragma("GCC diagnostic error \"-Wpadded\"")
#define END_ENSURE_PACKED _Pragma("GCC diagnostic pop")
#endif

#ifndef offsetof
#define offsetof(s, m) ((size_t)&(((s*)NULL)->m))
#endif

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

static inline float randomNormalizedFloat() { return (float)rand() / (float)RAND_MAX; }
