// Ludovico Maria Spitaleri 0001114169

#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>

/**
 * Parse command line arguments.
 */
typedef struct Args {
    size_t n_clusters;
    char* input_file_path;
    char* output_file_path;
} Args;

Args parse_cli_args(int argc, char* argv[]);

#endif  // ARGS_H
