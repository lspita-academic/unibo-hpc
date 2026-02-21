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
#include "points.h"
#include "safety.h"
#include "utils.h"

#define MAX_ITER 100
#define TOL 1e-5

int main(int argc, char* argv[]) {
    CliArgs args = parse_cli_args(argc, argv);

    printf("K: %lu\n", args.k);
    printf("Input file: %s\n", args.input_file_path);
    printf("Output file: %s\n\n", args.output_file_path);

    FILE* input_file = safe_fopen(args.input_file_path, "r");

    PointsCollection points = read_points_collection(input_file);
    fclose(input_file);

    printf("Points: %lu\n", points.size);
    printf("Dims: %lu\n\n", points.dimensions);

    FILE* output_file = safe_fopen(args.output_file_path, "w");
    for (size_t i = 0; i < points.size; i++) {
        for (size_t j = 0; j < points.dimensions; j++) {
            fprintf(
                output_file,
                POINT_COORD_PRINT_FORMAT " ",
                points.data[flat_index(i, j, points.dimensions)]
            );
        }
        fputc('\n', output_file);
    }
    fclose(output_file);

    points_collection_free(&points);

    return EXIT_SUCCESS;
}
