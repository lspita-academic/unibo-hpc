#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>

typedef struct {
    size_t k;
    char* input_file_path;
    char* output_file_path;
} CliArgs;

CliArgs parse_cli_args(int argc, char* argv[]);

#endif  // ARGS_H
