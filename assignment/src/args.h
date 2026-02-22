#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>

typedef struct Args {
    size_t n_clusters;
    char* input_file_path;
    char* output_file_path;
} Args;

Args parse_cli_args(int argc, char* argv[]);

#endif  // ARGS_H
