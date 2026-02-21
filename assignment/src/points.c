#include "points.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "safety.h"

#define STREAM_ROW_BUFLEN 1024

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

void read_points_collection_dimensions(
    FILE* stream, size_t* out_points, size_t* out_dims
) {
    char row_buffer[STREAM_ROW_BUFLEN];
    size_t points = 0;
    size_t dims = 0;

    size_t current_dims = 0;
    while (fgets(row_buffer, STREAM_ROW_BUFLEN, stream) != NULL) {
        size_t n_read_chars = strlen(row_buffer);
        if (n_read_chars == 0) {
            continue;
        }

        point_coord item;
        size_t item_offset = 0;
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

        /*
         * `fgets` could stop before then newline/EOF if the buffer is not large
         * enough.
         *
         * From `fgets` man pages:
         * fgets() reads in at most one less than size characters from stream
         * and stores them into the buffer  pointed to  by s. Reading stops
         * after an EOF or a newline. If a newline is read, it is stored into
         * the buffer. A terminating null byte ('\0') is stored after the last
         * character in the buffer.
         */
        if (row_buffer[n_read_chars - 1] == '\n') {
            update_dimensions(current_dims, &points, &dims);
            current_dims = 0;
        }
    }

    // Handle last row finishing with EOF instead of newline
    if (current_dims > 0) {
        update_dimensions(current_dims, &points, &dims);
    }

    *out_points = points;
    *out_dims = dims;
}

point_coord* read_points_collection_items(
    FILE* stream, size_t points, size_t dims
) {
    point_coord* items = safe_malloc(sizeof(*items) * points * dims);

    for (size_t i = 0; i < points * dims; i++) {
        int nread = fscanf(stream, POINT_COORD_READ_FORMAT, &items[i]);
        safe_assert(nread == 1, "Failed to read input file item\n");
    }
    return items;
}

PointsCollection read_points_collection(FILE* input_file) {
    size_t points = 0;
    size_t dims = 0;

    read_points_collection_dimensions(input_file, &points, &dims);
    rewind(input_file);
    point_coord* items = read_points_collection_items(input_file, points, dims);

    return (PointsCollection){
        .size = points, .dimensions = dims, .data = items
    };
}

void points_collection_free(PointsCollection* points) { free(points->data); }
