// Ludovico Maria Spitaleri 0001114169

#include "abort.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpi-abort.h"

#pragma weak mpi_safe_exit

void safe_exit(int status, char* message, ...) {
    if (message != NULL) {
        // collect variable arguments to pass them to fprintf
        // https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.5/html_node/Variable-Arguments-Output.html
        va_list ap;
        va_start(ap, message);  // message is the last fixed argument
        vfprintf(stderr, message, ap);
        va_end(ap);
    }
    if (mpi_safe_exit != NULL) {
        // MPI library is linked
        mpi_safe_exit(status);
    } else {
        exit(status);
    }
}

void safe_assert(bool condition, char* message, ...) {
    if (condition) {
        return;
    }
    safe_exit(EXIT_FAILURE, message == NULL ? "Assertion failed\n" : message);
}
