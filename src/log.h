#pragma once

#include <stdio.h>

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_WHITE    "\x1b[37m"
#define ANSI_COLOR_RESET    "\x1b[0m"

#define logNotification(format, ...) { printf("%s", ANSI_COLOR_WHITE);\
    printf((format), __VA_ARGS__);\
    printf("%s", ANSI_COLOR_RESET); }

#define logWarning(format, ...) { printf("%s", ANSI_COLOR_YELLOW);\
    printf((format), __VA_ARGS__);\
    printf("%s", ANSI_COLOR_RESET); }

#define logError(format, ...) { fprintf(stderr, "%s", ANSI_COLOR_RED);\
    fprintf(stderr, (format), __VA_ARGS__);\
    fprintf(stderr, "%s", ANSI_COLOR_RESET); }
