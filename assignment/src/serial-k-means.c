// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

#include <hpc.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "io.h"
#include "safety.h"

#define MAX_ITER 100
#define TOL 1e-5

int main(int argc, char* argv[]) {
    CliArgs args = parse_cli_args(argc, argv);

    printf("K: %lu\n", args.k);
    printf("Input file: %s\n", args.input_file_path);
    printf("Output file: %s\n", args.output_file_path);

    FILE* input_file = safe_fopen(args.input_file_path, "r");
    FILE* output_file = safe_fopen(args.output_file_path, "w");

    InputData input_data = read_input_file(input_file);

    fclose(input_file);
    fclose(output_file);
    return EXIT_SUCCESS;
}
