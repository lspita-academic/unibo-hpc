// Ludovico Maria Spitaleri 0001114169

#include "mpi-points.h"

MPI_Datatype mpi_point_type(size_t dimensions) {
    MPI_Datatype point_type;
    MPI_Type_contiguous(dimensions, MPI_POINT_COORD, &point_type);
    MPI_Type_commit(&point_type);
    return point_type;
}
