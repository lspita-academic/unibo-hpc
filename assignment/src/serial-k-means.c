// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#include <stddef.h>

#include "utils.h"
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
    printf("Output file: %s\n\n", args.output_file_path);

    FILE* input_file = safe_fopen(args.input_file_path, "r");

    InputData input_data = read_input_file(input_file);
    fclose(input_file);

    printf("Points: %lu\n", input_data.points);
    printf("Dims: %lu\n\n", input_data.dims);

    FILE* output_file = safe_fopen(args.output_file_path, "w");
    for (size_t i = 0; i < input_data.points; i++) {
        for (size_t j = 0; j < input_data.dims; j++) {
            fprintf(
                output_file,
                INPUT_ITEM_PRINT_FORMAT " ",
                input_data.items[flat_index(i, j, input_data.dims)]
            );
        }
        fputc('\n', output_file);
    }
    fclose(output_file);

    input_data_free(&input_data);

    return EXIT_SUCCESS;
}
