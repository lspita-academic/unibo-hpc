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
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abort.h"
#include "env.h"
#include "files.h"
#include "points.h"

#define K_MEANS_MAX_ITER 100
#define K_MEANS_TOL 1e-5
#define MAKE_MOVIE_ENV_VAR "MAKE_MOVIE"
#define DEMO_DIR_ENV_VAR "DEMO_DIR"

char* check_cli_arg(char* arg, char* arg_name) {
    safe_assert(
        strlen(arg) > 0, "Invalid argument %s: value is empty\n", arg_name
    );
    return arg;
}

KMeansArgs get_args(int argc, char* argv[]) {
    safe_assert(
        argc == 4, "Usage: %s n_clusters input_file output_file\n", argv[0]
    );

    char* n_clusters_str = check_cli_arg(argv[1], "n_clusters");
    char* input_file_path = check_cli_arg(argv[2], "input_file");
    char* output_file_path = check_cli_arg(argv[3], "output_file");

    /*
     * cast argument to unsigned long, without crashing on failure.
     * https://man7.org/linux/man-pages/man3/strtoul.3.html
     * In particular, if *nptr is not '\0' but **endptr is '\0' on return, the
     * entire string is valid.
     */
    char* endptr;
    size_t n_clusters = strtol(n_clusters_str, &endptr, 10);
    safe_assert(
        *endptr == '\0' && n_clusters >= 0,
        "Invalid argument n_clusters: value must be a non-negative number\n"
    );

    bool make_movie = get_env_bool(MAKE_MOVIE_ENV_VAR, false);
    char* demo_dir = get_env_string(DEMO_DIR_ENV_VAR, NULL);

    safe_assert(
        !make_movie || demo_dir != NULL,
        "If " MAKE_MOVIE_ENV_VAR " env var is set, " DEMO_DIR_ENV_VAR
        " env variable must be set to create a demo movie\n"
    );

    return (KMeansArgs){
        .n_clusters = n_clusters,
        .input_file_path = input_file_path,
        .output_file_path = output_file_path,
        .max_iterations = K_MEANS_MAX_ITER,
        .tolerance = K_MEANS_TOL,
        .make_movie = make_movie,
        .demo_dir = demo_dir,
    };
}

PointsCollection read_input_file(KMeansArgs* args) {
    FILE* input_file = safe_fopen(args->input_file_path, "r");
    PointsCollection points = read_points_collection(input_file);
    fclose(input_file);

    safe_assert(
        !args->make_movie || points.dimensions == 2,
        "Input points must have 2 dimensions to create a demo movie\n"
    );

    return points;
}

void print_inputs(FILE* stream, KMeansArgs* args, PointsCollection* points) {
    fprintf(stream, "Input file....... %s\n", args->input_file_path);
    fprintf(stream, "Output file...... %s\n", args->output_file_path);
    fprintf(stream, "Data points (N).. %lu\n", points->size);
    fprintf(stream, "Dimensions (D)... %lu\n", points->dimensions);
    fprintf(stream, "Clusters (K)..... %lu\n\n", args->n_clusters);
}

ClustersCollection create_clusters(KMeansArgs* args, PointsCollection* points) {
    ClustersCollection clusters =
        new_clusters_collection(points, args->n_clusters);
    init_centroids(&clusters);
    return clusters;
}

LoopData create_loop_data(void) {
    return (LoopData){
        .maxsqshift = 0,
        .iteration = 0,
        .start_time = hpc_gettime(),
    };
}

void reset_iteration(LoopData* loop) { loop->maxsqshift = 0; }

void print_iteration(FILE* stream, LoopData* loop) {
    fprintf(
        stream,
        "Iteration %3lu, maxsqshift = %" POINT_DISTANCE_FORMAT "\n",
        loop->iteration,
        loop->maxsqshift
    );
}

bool continue_loop(LoopData* loop, KMeansArgs* args) {
    return (loop->maxsqshift > args->tolerance * args->tolerance) &&
           (loop->iteration <= args->max_iterations);
}

double finish_loop(FILE* stream, LoopData* loop) {
    double elapsed = hpc_gettime() - loop->start_time;
    fprintf(stream, "\nMain loop completed\n");
    fprintf(stream, "Elapsed seconds %.3f\n\n", elapsed);
    return elapsed;
}

void write_output_file(KMeansArgs* args, ClustersCollection* clusters) {
    FILE* output_file = safe_fopen(args->output_file_path, "w");
    print_clusters_collection(output_file, clusters);
    fclose(output_file);
}
