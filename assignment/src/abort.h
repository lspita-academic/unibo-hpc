// Ludovico Maria Spitaleri 0001114169

#ifndef ABORT_H
#define ABORT_H

#include <stdbool.h>

/**
 * Safely exit with the given status code and message.
 */
void safe_exit(int status, char* message, ...);

/**
 * Assert the given condition, exit on failure.
 */
void safe_assert(bool condition, char* message, ...);

#endif  // ABORT_H
