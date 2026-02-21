#include "io.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "safety.h"

#define INPUT_FILE_ROW_BUFLEN 1024

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

void read_input_file_dimensions(
    FILE* input_file, size_t* out_points, size_t* out_dims
) {
    char row_buffer[INPUT_FILE_ROW_BUFLEN];
    size_t points = 0;
    size_t dims = 0;

    size_t current_dims = 0;
    while (fgets(row_buffer, INPUT_FILE_ROW_BUFLEN, input_file) != NULL) {
        size_t n_read_chars = strlen(row_buffer);
        if (n_read_chars == 0) {
            continue;
        }

        input_item_t item;
        size_t item_offset = 0;
        int item_chars = 0;
        while (sscanf(
                   row_buffer + item_offset,
                   INPUT_ITEM_READ_FORMAT "%n",
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

input_item_t* read_input_file_items(
    FILE* input_file, size_t points, size_t dims
) {
    input_item_t* items = safe_malloc(sizeof(*items) * points * dims);

    for (size_t i = 0; i < points * dims; i++) {
        int nread = fscanf(input_file, INPUT_ITEM_READ_FORMAT, &items[i]);
        safe_assert(nread == 1, "Failed to read input file item\n");
    }
    return items;
}

InputData read_input_file(FILE* input_file) {
    size_t points = 0;
    size_t dims = 0;

    read_input_file_dimensions(input_file, &points, &dims);
    rewind(input_file);
    input_item_t* items = read_input_file_items(input_file, points, dims);

    return (InputData){.points = points, .dims = dims, .items = items};
}

void input_data_free(InputData* data) { free(data->items); }
