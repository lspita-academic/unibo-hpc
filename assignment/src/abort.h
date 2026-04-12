// Ludovico Maria Spitaleri 0001114169

#ifndef ABORT_H
#define ABORT_H

#include <stdbool.h>

/**
 * Function to exit a program with the given status code.
 */
typedef void (*safe_exit_fn_t)(int);

/*
 * Set the exit function used to abort the program.
 */
void set_exit_function(safe_exit_fn_t safe_exit_fn);

/**
 * Safely exit with the given status code and message.
 */
void safe_exit(int status, char* message, ...);

/**
 * Assert the given condition, exit on failure.
 */
void safe_assert(bool condition, char* message, ...);

#endif  // ABORT_H
