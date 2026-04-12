// Ludovico Maria Spitaleri 0001114169

#ifndef MPI_UTILS_H
#define MPI_UTILS_H

#include <mpi.h>
#include <stdbool.h>

#define MPI_MASTER_RANK 0
#define MPI_DEFAULT_COMM MPI_COMM_WORLD
#define MPI_DEFAULT_TAG 0

#define MPI_SIZE_T MPI_UNSIGNED_LONG

/*
 * Check if the current mpi process is the master.
 */
bool mpi_is_master(int rank);

#endif // MPI_UTILS_H
