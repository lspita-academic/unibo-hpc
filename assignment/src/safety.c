// Ludovico Maria Spitaleri 0001114169

#include "safety.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi-utils.h"

#pragma weak mpi_safe_exit

void safe_exit(int status, char* message, ...) {
    if (message != NULL) {
        // collect variable arguments to pass them to fprintf
        // https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.5/html_node/Variable-Arguments-Output.html
        va_list ap;
        va_start(ap, message);  // message is the last fixed argument
        vfprintf(stderr, message, ap);
        va_end(ap);
    }
    if (mpi_safe_exit != NULL) {
        mpi_safe_exit(status);
    } else {
        exit(status);
    }
}

void safe_assert(bool condition, char* message, ...) {
    if (condition) {
        return;
    }
    safe_exit(EXIT_FAILURE, message == NULL ? "Assertion failed\n" : message);
}

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    safe_assert(ptr != NULL, "Failed to allocate %lu bytes\n", size);
    return ptr;
}

void* safe_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        safe_free(ptr);
        safe_exit(
            EXIT_FAILURE,
            "Failed to reallocate address %p to %lu bytes\n",
            ptr,
            size
        );
    }
    return new_ptr;
}

void* safe_free(void* ptr) {
    free(ptr);
    return NULL;
}

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
