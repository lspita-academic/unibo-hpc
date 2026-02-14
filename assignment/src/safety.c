// Ludovico Maria Spitaleri 0001114169

#include "safety.h"

#include <stdlib.h>

#include "mpi-utils.h"

#pragma weak mpi_safe_exit

void safe_exit(int status) {
  if (mpi_safe_exit != NULL) {
    mpi_safe_exit(status);
  } else {
    exit(status);
  }
}

void* safe_malloc(size_t size) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    safe_exit(EXIT_FAILURE);
  }
  return ptr;
}
