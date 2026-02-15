// Ludovico Maria Spitaleri 0001114169

#ifndef SAFETY_H
#define SAFETY_H

#include <stdbool.h>
#include <stdlib.h>

void safe_exit(int status);
void safe_assert(bool condition, char* message, ...);
void* safe_malloc(size_t size);

#endif  // SAFETY_H
