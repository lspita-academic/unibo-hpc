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

/**
 * Read a line of any length from a file stream.
 * If `out_buffer` is NULL, the function will allocate memory for the line,
 * otherwise it will use the provided buffer. This allows to allocate memory
 * only when needed, and free it only after the read loop is completed.
 *
 * Returns the length of the line, including the newline character.
 */
size_t read_file_line(FILE* stream, char** out_buffer);

#endif  // SAFETY_H
