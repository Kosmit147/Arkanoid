#pragma once

#include <stdio.h>

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_WHITE    "\x1b[37m"
#define ANSI_COLOR_RESET    "\x1b[0m"

static inline void logNotification(const char* format, const char* message, const char* color)
{
    printf("%s", color);
    printf(format, message);
    printf("%s", ANSI_COLOR_RESET);
}

static inline void logError(const char* format, const char* message, const char* color)
{
    fprintf(stderr, "%s", color);
    fprintf(stderr, format, message);
    fprintf(stderr, "%s", ANSI_COLOR_RESET);
}