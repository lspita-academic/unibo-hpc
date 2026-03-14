// Ludovico Maria Spitaleri 0001114169

#ifndef ABORT_H
#define ABORT_H

#include <stdbool.h>

/**
 * Function to exit a program with the given status code.
 * Create a global function pointer of this type named `safe_exit_fn` to
 * override the default exit function used by `safe_exit`.
 */
typedef void (*safe_exit_fn_t)(int);

/**
 * Safely exit with the given status code and message.
 */
void safe_exit(int status, char* message, ...);

/**
 * Assert the given condition, exit on failure.
 */
void safe_assert(bool condition, char* message, ...);

#endif  // ABORT_H
