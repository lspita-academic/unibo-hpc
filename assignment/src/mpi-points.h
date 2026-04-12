// Ludovico Maria Spitaleri 0001114169

#ifndef MPI_POINTS_H
#define MPI_POINTS_H

#include <mpi.h>
#include <stddef.h>

#define MPI_POINT_COORD MPI_DOUBLE
#define MPI_POINT_DISTANCE MPI_POINT_COORD

/*
 * Create an mpi data type to represent a point.
 */
MPI_Datatype mpi_point_type(size_t dimensions);

#endif // MPI_POINTS_H
