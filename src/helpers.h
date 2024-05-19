#pragma once

#include <stdlib.h>
#include <string.h>

#if defined(__GNUC__) || defined(__clang__)
#define UNUSED(x) unused_ ## x __attribute__((__unused__))
#else
#define UNUSED(x) unused_ ## x
#endif

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(low, high, x) (max((low), min((x), (high))))
#define withinRange(low, high, x) ((x) >= (low) && (x) <= (high))

static inline float randomNormalizedFloat() { return (float)rand() / (float)RAND_MAX; }
