#pragma once

#include <stdio.h>

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_WHITE    "\x1b[37m"
#define ANSI_COLOR_RESET    "\x1b[0m"

// use like printf
#define logNotification(...) printf(__VA_ARGS__);

// use like printf
#define logWarning(...) { printf("%s", ANSI_COLOR_YELLOW);\
    printf(__VA_ARGS__);\
    printf("%s", ANSI_COLOR_RESET); }

// use like printf
#define logError(...) { fprintf(stderr, "%s", ANSI_COLOR_RED);\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "%s", ANSI_COLOR_RESET); }
