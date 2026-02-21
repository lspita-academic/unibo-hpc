#ifndef IO_H
#define IO_H

#include <stddef.h>
#include <stdio.h>

#define INPUT_ITEM_READ_FORMAT "%lf"
#define INPUT_ITEM_PRINT_FORMAT "%lf"

typedef double input_item_t;

typedef struct InputData {
    size_t points;
    size_t dims;
    input_item_t* items;
} InputData;

InputData read_input_file(FILE* input_file);
void input_data_free(InputData* data);

#endif  // IO_H
