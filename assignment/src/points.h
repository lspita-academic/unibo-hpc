// Ludovico Maria Spitaleri 0001114169

#ifndef POINTS_H
#define POINTS_H

#include <stddef.h>
#include <stdio.h>

#define POINT_COORD_READ_FORMAT "%lf"
#define POINT_COORD_PRINT_FORMAT "%lf"

typedef double point_coord;

typedef struct PointsCollection {
    size_t size;
    size_t dimensions;
    point_coord* data;
} PointsCollection;

PointsCollection read_points_collection(FILE* stream);
void points_collection_free(PointsCollection* points);
void print_points_collection(FILE* stream, PointsCollection* points);

#endif  // IO_H
