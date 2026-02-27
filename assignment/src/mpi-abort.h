// Ludovico Maria Spitaleri 0001114169

#ifndef MPI_ABORT_H
#define MPI_ABORT_H

/**
 * Safely exit all MPI processes with the given status code.
 */
void mpi_safe_exit(int status);

#endif  // MPI_ABORT_H
