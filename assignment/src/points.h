// Ludovico Maria Spitaleri 0001114169

#ifndef POINTS_H
#define POINTS_H

#include <stddef.h>
#include <stdio.h>

/**
 * Point's single coordinate value.
 */
typedef double point_coord;

#define POINT_COORD_READ_FORMAT "%lf"
#define POINT_COORD_PRINT_FORMAT "%lf"

/**
 * Array of `size` points each of `dimensions` dimensions.
 */
typedef struct PointsArray {
    size_t size;
    size_t dimensions;
    point_coord* data;
} PointsArray;

/**
 * Create a `PointsArray`.
 */
PointsArray new_points_array(size_t size, size_t dimensions, point_coord* data);

/**
 * Free the memory allocated for the fields of a `PointsArray`.
 */
void free_points_array(PointsArray* points);

/**
 * Read an array of points from a file stream.
 * Each line of the input must contain the coordinates of a point.
 * Each point must have the same number of dimensions.
 * Empty lines are ignored.
 */
PointsArray read_points_array(FILE* stream);

/**
 * Print a `PointsArray` to a file stream.
 */
void print_points_array(FILE* stream, PointsArray* points);

#endif  // POINTS_H
