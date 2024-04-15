#pragma once

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define clamp(low, high, x) (max((low), min((x), (high))))
#define withinRange(low, high, x) ((x) >= (low) && (x) <= (high))