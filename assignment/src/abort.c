// Ludovico Maria Spitaleri 0001114169

#include "abort.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Symbol to override the default exit function used by safe_exit.
 */
extern safe_exit_fn_t safe_exit_fn;

// https://gcc.gnu.org/onlinedocs/gcc/Weak-Pragmas.html
#pragma weak safe_exit_fn

/**
 * va_list is a type used for variable function arguments.
 * https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.5/html_node/Variable-Arguments-Output.html
 */
void vsafe_exit(int status, char* message, va_list ap) {
    if (message != NULL) {
        vfprintf(stderr, message, ap);
        va_end(ap);
    }
    // use special exit function or default exit
    if (&safe_exit_fn != NULL) {
        safe_exit_fn(status);
    } else {
        exit(status);
    }
}

void safe_exit(int status, char* message, ...) {
    va_list ap;
    va_start(ap, message);  // message is the last fixed argument
    vsafe_exit(status, message, ap);
}

void safe_assert(bool condition, char* message, ...) {
    if (condition) {
        return;
    }
    message = message == NULL ? "Assertion failed\n" : message;
    int status = EXIT_FAILURE;

    va_list ap;
    va_start(ap, message);  // message is the last fixed argument
    vsafe_exit(status, message, ap);
}
