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

#include "args.h"
#include "files.h"
#include "points.h"

#define MAX_ITER 100
#define TOL 1e-5

/**
 * Serial implementation of k-means clustering.
 */
int main(int argc, char* argv[]) {
    Args args = parse_cli_args(argc, argv);

    FILE* input_file = safe_fopen(args.input_file_path, "r");

    PointsArray points = read_points_array(input_file);
    fclose(input_file);

    FILE* output_file = safe_fopen(args.output_file_path, "w");
    print_points_array(output_file, &points);
    fclose(output_file);

    points_array_free(&points);

    return EXIT_SUCCESS;
}
