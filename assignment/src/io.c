#include "io.h"

#include <stdio.h>
#include <stdlib.h>

#include "safety.h"
#include "utils.h"

#define INPUT_FILE_BUFLEN 1024

InputData read_input_file(FILE* input_file) {
    char buffer[INPUT_FILE_BUFLEN];
    InputData input_data = {0};

    /* Get the first line of the input file, and count how many
       numbers are there. This function is not very robust: if the
       first line is empty, the number of dimensions will be zero; if
       the first line has more than `BUFLEN` characters, the number of
       fields will be computed incorrectly. */
    char* i_dont_care = fgets(buffer, INPUT_FILE_BUFLEN, input_file);
    (void)i_dont_care; /* Avoid a compiler warning. */
    input_data.dims = -1;
    char *start, *end = buffer;
    do {
        start = end;
        strtof(start, &end);
        input_data.dims++;
    } while (end != start);

    safe_assert(input_data.dims > 0, "First line of input file is empty");

    /* Rewind the file and count how many data items are there. */
    rewind(input_file);
    int n_items = 0;
    float dummy;
    while (1 == fscanf(input_file, "%f", &dummy)) n_items++;

    safe_assert(
        input_data.points % input_data.dims == 0,
        "Lines of input file are not of regular length"
    );

    input_data.points = n_items / input_data.dims;

    input_data.data = (float*)safe_malloc(
        input_data.points * input_data.dims * sizeof(*input_data.data)
    );

    /* Rewind and read the actual data. */
    rewind(input_file);
    for (size_t i = 0; i < input_data.points; i++) {
        for (size_t j = 0; j < input_data.dims; j++) {
            const size_t idx = flat_index(i, j, input_data.dims);
            const int nread = fscanf(input_file, "%f", &input_data.data[idx]);
            safe_assert(nread == 1, "Failed to read input file data");
        }
    }

    return input_data;
}
