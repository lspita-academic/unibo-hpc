// Ludovico Maria Spitaleri 0001114169

#include "mpi-utils.h"

bool mpi_is_master(int rank) {
    return rank == MPI_MASTER_RANK;
}
