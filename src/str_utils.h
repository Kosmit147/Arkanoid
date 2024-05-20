#pragma once

#include <stdio.h>

#define staticStrLen(str) (sizeof(str) / sizeof(char) - 1)

// returns the number of characters which were written excluding the null terminator
// buffSize means the whole buffer, including the null terminator
static inline size_t uiToStr(unsigned int val, char* str, size_t buffSize)
{
    size_t charsWritten = (size_t)snprintf(str, buffSize, "%u", val);

    if (charsWritten > buffSize - 1)
        charsWritten = buffSize - 1;

    return charsWritten;
}
