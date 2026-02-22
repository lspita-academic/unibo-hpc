// Ludovico Maria Spitaleri 0001114169

#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

/**
 * Allocate memory, exit on failure.
 */
void* safe_malloc(size_t size);

/**
 * Free pointer and return NULL.
 * Useful to do `ptr = safe_free(ptr);`.
 */
void* safe_free(void* ptr);

/**
 * Reallocate memory pointer, exit on failure.
 */
void* safe_realloc(void* ptr, size_t size);

#endif  // MEMORY_H
