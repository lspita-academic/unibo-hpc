#include "io.h"

#include <stdio.h>
#include <stdlib.h>

#include <cstddef>

#include "safety.h"
#include "utils.h"

#define BUFLEN 1024

InputData read_input_file(FILE* input_file) {
  char buffer[BUFLEN];
  InputData result = {0};

  /* Get the first line of the input file, and count how many
     numbers are there. This function is not very robust: if the
     first line is empty, the number of dimensions will be zero; if
     the first line has more than `BUFLEN` characters, the number of
     fields will be computed incorrectly. */
  char* i_dont_care = fgets(buffer, BUFLEN, input_file);
  (void)i_dont_care; /* Avoid a compiler warning. */
  result.dims = -1;
  char *start, *end = buffer;
  do {
    start = end;
    strtof(start, &end);
    result.dims++;
  } while (end != start);

  safe_assert(result.dims > 0, "First line of input file is empty");

  /* Rewind the file and count how many data items are there. */
  rewind(input_file);
  int n_items = 0;
  float dummy;
  while (1 == fscanf(input_file, "%f", &dummy)) n_items++;

  safe_assert(
      result.points % result.dims == 0,
      "Lines of input file are not of regular length"
  );

  result.points = n_items / result.dims;

  result.data =
      (float*)safe_malloc(result.points * result.dims * sizeof(*result.data));

  /* Rewind and read the actual data. */
  rewind(input_file);
  for (size_t i = 0; i < result.points; i++) {
    for (size_t j = 0; j < result.dims; j++) {
      const size_t idx = flat_index(i, j, result.dims);
      const int nread = fscanf(input_file, "%f", &result.data[idx]);
      safe_assert(nread == 1, "Failed to read input file data");
    }
  }

  return result;
}
