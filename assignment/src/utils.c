#include "utils.h"

#include <stddef.h>

size_t flat_index(size_t i, size_t j, size_t row_length) {
  return i * row_length + j;
}
