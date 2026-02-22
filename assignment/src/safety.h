// Ludovico Maria Spitaleri 0001114169

#ifndef SAFETY_H
#define SAFETY_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Safely exit with the given status code.
 */
void safe_exit(int status);

/**
 * Assert the given condition and exit the program on failure.
 */
void safe_assert(bool condition, char* message, ...);

/**
 * Allocate memory and exit on failure.
 */
void* safe_malloc(size_t size);

/**
 * Open a file and exit on failure.
 */
FILE* safe_fopen(char* path, char* mode);

/**
 * Close a file and exit on failure.
 */
void safe_fclose(FILE* file);

#endif  // SAFETY_H
