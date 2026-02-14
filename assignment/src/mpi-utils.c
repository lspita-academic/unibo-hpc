#include "mpi-utils.h"

#include <mpi.h>
#include <stdlib.h>

void mpi_safe_exit(int status) {
  int flag;
  MPI_Initialized(&flag);
  if (flag) {
    MPI_Abort(MPI_COMM_WORLD, status);
  } else {
    exit(status);
  }
}
