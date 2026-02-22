// Ludovico Maria Spitaleri 0001114169

#include "points.h"

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abort.h"
#include "files.h"
#include "memory.h"
#include "utils.h"

/**
 * Check and update the dimensions of the points array.
 */
void update_dimensions(
    size_t current_dims, size_t* out_points, size_t* out_dims
) {
    size_t dims = *out_dims;
    size_t points = *out_points;

    if (dims == 0) {
        dims = current_dims;
    } else {
        safe_assert(
            current_dims == dims,
            "Input file points have irregular dimensions\n"
        );
    }
    points++;

    *out_points = points;
    *out_dims = dims;
}

void read_points_array_dimensions(
    FILE* stream, size_t* out_points, size_t* out_dims
) {
    char* row_buffer = NULL;
    size_t points = 0;
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
                   POINT_COORD_READ_FORMAT "%n",
                   &item,
                   &item_chars
               ) > 0) {
            current_dims++;
            item_offset += item_chars;
        }

        update_dimensions(current_dims, &points, &dims);
        current_dims = 0;
    }
    row_buffer = safe_free(row_buffer);

    // Handle last row finishing with EOF instead of newline
    if (current_dims > 0) {
        update_dimensions(current_dims, &points, &dims);
    }

    *out_points = points;
    *out_dims = dims;
}

point_coord* read_points_array_items(FILE* stream, size_t points, size_t dims) {
    point_coord* items = safe_malloc(sizeof(*items) * points * dims);

    for (size_t i = 0; i < points * dims; i++) {
        int nread = fscanf(stream, POINT_COORD_READ_FORMAT, &items[i]);
        safe_assert(nread == 1, "Failed to read input file item\n");
    }
    return items;
}

PointsArray read_points_array(FILE* stream) {
    size_t points = 0;
    size_t dims = 0;

    read_points_array_dimensions(stream, &points, &dims);
    rewind(stream);
    point_coord* items = read_points_array_items(stream, points, dims);

    return (PointsArray){.size = points, .dimensions = dims, .data = items};
}

void points_array_free(PointsArray* points) {
    points->data = safe_free(points->data);
}

void print_points_array(FILE* stream, PointsArray* points) {
    for (size_t i = 0; i < points->size; i++) {
        for (size_t j = 0; j < points->dimensions; j++) {
            size_t idx = flat_index(i, j, points->dimensions);
            fprintf(stream, POINT_COORD_PRINT_FORMAT " ", points->data[idx]);
        }
        fputc('\n', stream);
    }
}
