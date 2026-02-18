#ifndef IO_H
#define IO_H

#include <stddef.h>
#include <stdio.h>

typedef struct InputData {
  size_t points;
  size_t dims;
  float* data;
} InputData;

InputData read_input_file(FILE* input_file);

#endif  // IO_H
