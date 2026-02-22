// Ludovico Maria Spitaleri 0001114169

#include "memory.h"

#include <stdlib.h>

#include "abort.h"

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    safe_assert(ptr != NULL, "Failed to allocate %lu bytes\n", size);
    return ptr;
}

void* safe_free(void* ptr) {
    free(ptr);
    return NULL;
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
