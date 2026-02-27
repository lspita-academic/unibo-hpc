// Ludovico Maria Spitaleri 0001114169

#include "mpi-abort.h"

#include <mpi.h>
#include <stdlib.h>

void mpi_safe_exit(int status) {
    int initialized;
    MPI_Initialized(&initialized);
    if (initialized) {
        MPI_Abort(MPI_COMM_WORLD, status);
    } else {
        exit(status);
    }
}
