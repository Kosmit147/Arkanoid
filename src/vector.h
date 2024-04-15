#pragma once

#include <stdlib.h>
#include <string.h>

typedef struct Vector
{
    void* data;
    size_t size; // in bytes
    size_t allocatedSize; // in bytes
} Vector;

inline static Vector vectorCreate()
{
    Vector vector = {
        .data = NULL,
        .size = 0,
        .allocatedSize = 0,
    };

    return vector;
}

static inline void vectorRealloc(Vector* vector, size_t newSize)
{
    vector->data = realloc(vector->data, newSize);
    vector->allocatedSize = newSize;
}

static inline void vectorReserve(Vector* vector, size_t newElemCount, size_t elemSize)
{
    size_t newSize = newElemCount * elemSize;

    if (newSize <= vector->allocatedSize)
        return;

    vectorRealloc(vector, newSize);
}

static inline void* vectorGet(Vector* vector, size_t index, size_t elemSize)
{
    return (void*)(((char*)(vector->data)) + index * elemSize);
}

static inline void* vectorPushBack(Vector* vector, const void* elem, size_t elemSize)
{
    if (vector->size + elemSize > vector->allocatedSize)
        vectorRealloc(vector, vector->allocatedSize * 2 + elemSize);

    void* newElemPtr = vectorGet(vector, vector->size, sizeof(char));
    memcpy(newElemPtr, elem, elemSize);
    vector->size += elemSize;

    return newElemPtr;
}

static inline void vectorClear(Vector* vector)
{
    vector->size = 0;
}

static inline void vectorFree(Vector* vector)
{
    free(vector->data);
    vector->size = 0;
    vector->allocatedSize = 0;
}