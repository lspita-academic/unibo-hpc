// Ludovico Maria Spitaleri 0001114169

#ifndef SAFETY_H
#define SAFETY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Safely exit with the given status code and message.
 */
void safe_exit(int status, char* message, ...);

/**
 * Assert the given condition, exit on failure.
 */
void safe_assert(bool condition, char* message, ...);

/**
 * Allocate memory, exit on failure.
 */
void* safe_malloc(size_t size);

/**
 * Reallocate memory pointer, exit on failure.
 */
void* safe_realloc(void* ptr, size_t size);

/**
 * Free pointer and return NULL.
 * Useful to do `ptr = safe_free(ptr);`.
 */
void* safe_free(void* ptr);

/**
 * Open a file, exit on failure.
 */
FILE* safe_fopen(char* path, char* mode);

/**
 * Close a file, exit on failure.
 */
void safe_fclose(FILE* file);

#endif  // SAFETY_H
