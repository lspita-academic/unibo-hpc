// Ludovico Maria Spitaleri 0001114169

#ifndef POINTS_H
#define POINTS_H

#include <float.h>
#include <stddef.h>
#include <stdio.h>

/**
 * Point's single coordinate value.
 */
typedef double point_coord;

/**
 * Point's coordinates distance value.
 */
typedef point_coord point_distance;

#define POINT_COORD_OUT_FORMAT "f"
#define POINT_COORD_IN_FORMAT "lf"
#define POINT_COORD_MAX DBL_MAX
#define POINT_DISTANCE_OUT_FORMAT POINT_COORD_OUT_FORMAT
#define POINT_DISTANCE_IN_FORMAT POINT_COORD_IN_FORMAT
#define POINT_DISTANCE_MAX POINT_COORD_MAX

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
void points_copy(point_coord* dest, point_coord* src, size_t dimensions);

/**
 * Compute the euclidean squared distance between two points.
 */
point_distance points_distance(
    point_coord* p1, point_coord* p2, size_t dimensions
);

/**
 * Set all coordinates of a point to zero.
 */
void zero_point(point_coord* p, size_t dimensions);

/**
 * Sum the coordinates of point `src` to point `dest`.
 */
void points_add(point_coord* dest, point_coord* src, size_t dimensions);

/**
 * Multiply the coordinates of a point with a scalar.
 */
void point_scalar_mul(point_coord* p, point_coord s, size_t dimensions);

#endif  // POINTS_H
