#include "util.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../deps/dirent/dirent.h"

#if defined(_WIN32)
    #include <wchar.h>
    #include <direct.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

int ends_with(const char *str, const char *suffix) {
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int format_length(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t needed = vsnprintf(NULL, 0, fmt, args) + 1;
    va_end(args);
    return needed;
}

void cross_mkdir(const char* path) {
    #if defined(_WIN32)
        int needed = strlen(path) + 1;
        wchar_t* ws = malloc(sizeof(wchar_t) * needed);
        swprintf(ws, needed, L"%hs", path);
        _wmkdir(ws);
    #else
        mkdir(path, 0777);
    #endif
}

void cross_chdir(const char* path) {
    #if defined(_WIN32)
        int needed = strlen(path) + 1;
        wchar_t* ws = malloc(sizeof(wchar_t) * needed);
        swprintf(ws, needed, L"%hs", path);
        _wchdir(ws);
    #else
        chdir(path);
    #endif
}
