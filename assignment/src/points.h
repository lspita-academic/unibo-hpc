// Ludovico Maria Spitaleri 0001114169

#ifndef POINTS_H
#define POINTS_H

#include <stddef.h>
#include <stdio.h>

#define POINT_COORD_READ_FORMAT "%lf"
#define POINT_COORD_PRINT_FORMAT "%lf"

/**
 * Point's single coordinate value.
 */
typedef double point_coord;

/**
 * Array of `size` points each of `dimensions` dimensions.
 */
typedef struct PointsArray {
    size_t size;
    size_t dimensions;
    point_coord* data;
} PointsArray;

/**
 * Array of `size` clusters related to the `points` array.
 * The centroid of each cluster is stored in the `centroids` array.
 * The `cluster_of` array stores the cluster index of each point.
 * The `counts` array stores the number of points in each cluster.
 */
typedef struct ClustersArray {
    PointsArray* points;
    PointsArray centroids;
    size_t size;
    size_t* cluster_of;
    size_t* counts;
} ClustersArray;

/**
 * Read an array of points from a file stream.
 * Each line of the input must contain the coordinates of a point.
 * Each point must have the same number of dimensions.
 * Empty lines are ignored.
 */
PointsArray read_points_array(FILE* stream);

/**
 * Free the memory allocated for the fields of a `PointsArray`.
 */
void points_array_free(PointsArray* points);

/**
 * Print a `PointsArray` to a file stream.
 */
void print_points_array(FILE* stream, PointsArray* points);

#endif  // POINTS_H
