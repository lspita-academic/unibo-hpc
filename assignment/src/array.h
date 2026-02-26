// Ludovico Maria Spitaleri 0001114169

#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

/**
 * Convert a 2D index into a 1D index.
 */
size_t flat_index(size_t i, size_t j, size_t row_length);

#endif  // ARRAY_H
