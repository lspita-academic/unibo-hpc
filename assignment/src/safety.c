#include "safety.h"

#include <stdio.h>
#include <stdlib.h>

#include "mpi-utils.h"

#pragma weak mpi_safe_exit

void safe_exit(int status) {
  if (mpi_safe_exit != NULL) {
    puts("MPI");
    mpi_safe_exit(status);
  } else {
    puts("Default");
    exit(status);
  }
}
