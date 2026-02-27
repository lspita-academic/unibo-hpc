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
 * Collection of `size` points each of `dimensions` dimensions.
 */
typedef struct PointsCollection {
    size_t size;
    size_t dimensions;
    point_coord* data;
} PointsCollection;

/**
 * Create a `PointsCollection`.
 */
PointsCollection new_points_collection(
    size_t size, size_t dimensions, point_coord* data
);

/**
 * Free the memory allocated for the fields of a `PointsCollection`.
 */
void free_points_collection(PointsCollection* points);

/**
 * Read a collection of points from a file stream.
 * Each line of the input must contain the coordinates of a point.
 * Each point must have the same number of dimensions.
 * Empty lines are ignored.
 */
PointsCollection read_points_collection(FILE* stream);

/**
 * Print a `PointsCollection` to a file stream.
 */
void print_points_collection(FILE* stream, PointsCollection* points);

/**
 * Copy a point from `src` to `dest`.
 */
void copy_point(point_coord* dest, point_coord* src, size_t dimensions);

#endif  // POINTS_H
