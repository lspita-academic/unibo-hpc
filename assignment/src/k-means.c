// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

#include "k-means.h"

#include <hpc.h>
#include <stdio.h>
#include <stdlib.h>

#include "args.h"
#include "clusters.h"
#include "files.h"
#include "points.h"
#include "random.h"

#define K_MEANS_MAX_ITER 100
#define K_MEANS_TOL 1e-5

int k_means(
    int argc,
    char* argv[],
    classify_points_fn classify_points,
    update_centroids_fn update_centroids
) {
    init_random();
    Args args = parse_cli_args(argc, argv);

    FILE* input_file = safe_fopen(args.input_file_path, "r");
    PointsCollection points = read_points_collection(input_file);
    fclose(input_file);

    ClustersCollection clusters =
        new_clusters_collection(&points, args.n_clusters);
    init_centroids(&clusters);

    printf("Input file....... %s\n", args.input_file_path);
    printf("Output file...... %s\n", args.output_file_path);
    printf("Data points (N).. %lu\n", points.size);
    printf("Dimensions (D)... %lu\n", points.dimensions);
    printf("Clusters (K)..... %lu\n\n", clusters.size);

    point_distance maxsqshift;
    size_t iter = 0;
    double tstart = hpc_gettime();
    do {
        classify_points(&clusters);

        // MAKE_MOVIE

        maxsqshift = update_centroids(&clusters);
        printf(
            "Iteration %3lu, maxsqshift = %" POINT_DISTANCE_FORMAT "\n",
            iter,
            maxsqshift
        );
        iter++;
    } while ((maxsqshift > K_MEANS_TOL * K_MEANS_TOL) &&
             (iter <= K_MEANS_MAX_ITER));
    const double elapsed = hpc_gettime() - tstart;

    printf("\nMain loop completed\n");
    printf("Elapsed seconds %.3f\n\n", elapsed);

    FILE* output_file = safe_fopen(args.output_file_path, "w");
    print_clusters_collection(output_file, &clusters);
    fclose(output_file);

    free_clusters_collection(&clusters);
    free_points_collection(&points);

    return EXIT_SUCCESS;
}
