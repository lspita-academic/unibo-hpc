// Ludovico Maria Spitaleri 0001114169

#ifndef SAFETY_H
#define SAFETY_H

#include <stdio.h>

/**
 * Open a file, exit on failure.
 */
FILE* safe_fopen(char* path, char* mode);

/**
 * Close a file, exit on failure.
 */
void safe_fclose(FILE* file);

#endif  // SAFETY_H
