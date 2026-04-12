// Ludovico Maria Spitaleri 0001114169

#include "points.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abort.h"
#include "array.h"
#include "files.h"
#include "memory.h"

PointsCollection new_points_collection(
    size_t size, size_t dimensions, point_coord* data
) {
    data = data == NULL ? safe_malloc(sizeof(*data) * size * dimensions) : data;
    return (PointsCollection){
        .size = size, .dimensions = dimensions, .data = data
    };
}

/*
 * Check and update the point collection metadata.
 */
void update_metadata(
    size_t current_dims, size_t* out_n_points, size_t* out_dims
) {
    size_t dims = *out_dims;
    size_t n_points = *out_n_points;

    if (dims == 0) {
        dims = current_dims;
    } else {
        safe_assert(
            current_dims == dims,
            "Input file points have irregular dimensions: found %lu expected "
            "%lu\n",
            current_dims,
            dims
        );
    }
    n_points++;

    *out_n_points = n_points;
    *out_dims = dims;
}

/*
 * Read the points collection metadata needed to create the collection.
 */
void read_points_collection_metadata(
    FILE* stream, size_t* out_n_points, size_t* out_dims
) {
    char* row_buffer = NULL;
    size_t n_points = 0;
    size_t dims = 0;

    size_t current_dims = 0;
    while (read_file_line(stream, &row_buffer) > 0) {
        size_t n_read_chars = strlen(row_buffer);

        // trim out leading whitespace
        size_t start_idx = 0;
        for (; start_idx < n_read_chars && isspace(row_buffer[start_idx]);
             start_idx++) {
        }

        if (start_idx == n_read_chars) {
            // empty reading
            continue;
        }

        point_coord item;
        size_t item_offset = start_idx;
        int item_chars = 0;
        while (sscanf(
                   row_buffer + item_offset,
                   "%" POINT_COORD_IN_FORMAT "%n",
                   &item,
                   &item_chars
               ) > 0) {
            current_dims++;
            item_offset += item_chars;
        }

        update_metadata(current_dims, &n_points, &dims);
        current_dims = 0;
    }
    row_buffer = safe_free(row_buffer);

    // Handle last row finishing with EOF instead of newline
    if (current_dims > 0) {
        update_metadata(current_dims, &n_points, &dims);
    }

    *out_n_points = n_points;
    *out_dims = dims;
}

point_coord* read_points_collection_items(
    FILE* stream, size_t n_points, size_t dims
) {
    point_coord* items = safe_malloc(sizeof(*items) * n_points * dims);

    for (size_t i = 0; i < n_points * dims; i++) {
        int nread = fscanf(stream, "%" POINT_COORD_IN_FORMAT, &items[i]);
        safe_assert(nread == 1, "Failed to read input file item\n");
    }
    return items;
}

PointsCollection read_points_collection(FILE* stream) {
    size_t n_points = 0;
    size_t dims = 0;

    read_points_collection_metadata(stream, &n_points, &dims);
    rewind(stream);
    point_coord* items = read_points_collection_items(stream, n_points, dims);

    return new_points_collection(n_points, dims, items);
}

void free_points_collection(PointsCollection* points) {
    points->data = safe_free(points->data);
}

void print_points_collection(FILE* stream, PointsCollection* points) {
    size_t dims = points->dimensions;
    for (size_t i = 0; i < points->size; i++) {
        for (size_t j = 0; j < dims; j++) {
            size_t idx = flat_index(i, j, dims);
            fprintf(stream, "%" POINT_COORD_OUT_FORMAT " ", points->data[idx]);
        }
        fprintf(stream, "\n");
    }
}

void point_copy(point_coord* dest, point_coord* src, size_t dimensions) {
    /*
     * `memcpy` is thread-safe and allows to copy the data in one operation.
     * From memcpy(3) man pages:
     * ATTRIBUTES
     *      For an explanation of the terms used in this section, see attributes(7).
     *      ┌───────────┬───────────────┬─────────┐
     *      │ Interface │ Attribute     │ Value   │
     *      ├───────────┼───────────────┼─────────┤
     *      │ memcpy()  │ Thread safety │ MT-Safe │
     *      └───────────┴───────────────┴─────────┘
     */
    memcpy(dest, src, sizeof(*dest) * dimensions);
}

point_distance points_distance(
    point_coord* p1, point_coord* p2, size_t dimensions
) {
    point_distance distance = 0.0;
    for (size_t i = 0; i < dimensions; i++) {
        distance += (p1[i] - p2[i]) * (p1[i] - p2[i]);
    }
    return distance;
}

void zero_point(point_coord* p, size_t dimensions) {
    /*
     * `memset` is thread-safe and allows to set the data in one operation.
     * From memset(3) man pages:
     * ATTRIBUTES
     *      For an explanation of the terms used in this section, see attributes(7).
     *      ┌───────────┬───────────────┬─────────┐
     *      │ Interface │ Attribute     │ Value   │
     *      ├───────────┼───────────────┼─────────┤
     *      │ memset()  │ Thread safety │ MT-Safe │
     *      └───────────┴───────────────┴─────────┘
     */
    memset(p, 0, sizeof(*p) * dimensions);
}

void points_add(point_coord* dest, point_coord* src, size_t dimensions) {
    for (size_t i = 0; i < dimensions; i++) {
        dest[i] += src[i];
    }
}

void point_scalar_mul(point_coord* p, point_coord s, size_t dimensions) {
    for (size_t i = 0; i < dimensions; i++) {
        p[i] *= s;
    }
}
