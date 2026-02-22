// Ludovico Maria Spitaleri 0001114169

#include "files.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "abort.h"

FILE* safe_fopen(char* path, char* mode) {
    FILE* file = fopen(path, mode);
    safe_assert(
        file != NULL, "Failed to open file %s in %s mode\n", path, mode
    );
    return file;
}

void safe_fclose(FILE* file) {
    int result = fclose(file);
    safe_assert(result == 0, "Failed to close file: %s\n", strerror(errno));
}
