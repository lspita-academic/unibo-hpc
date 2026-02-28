// Ludovico Maria Spitaleri 0001114169

#ifndef MPI_ABORT_H
#define MPI_ABORT_H

#include "abort.h"

/**
 * Safely exit all MPI processes with the given status code.
 */
void mpi_safe_exit(int status);

/**
 * Override the default exit function used by `safe_exit`.
 */
exit_fn safe_exit_fn = mpi_safe_exit;

#endif  // MPI_ABORT_H
