#include "util.h"

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
