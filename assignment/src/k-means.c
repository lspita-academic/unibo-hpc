// Ludovico Maria Spitaleri 0001114169

#include "k-means.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "abort.h"
#include "env.h"
#include "files.h"
#include "points.h"
#include "array.h"

#define K_MEANS_MAX_ITER 100
#define K_MEANS_TOL 1e-5
#define FORCE_ITERATIONS_ENV_VAR "FORCE_ITERATIONS"
#define MAKE_MOVIE_ENV_VAR "MAKE_MOVIE"
#define MOVIE_DIR_ENV_VAR "MOVIE_DIR"

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

    bool force_iterations = get_env_bool(FORCE_ITERATIONS_ENV_VAR, false);
    bool make_movie = get_env_bool(MAKE_MOVIE_ENV_VAR, false);
    char* movie_dir = get_env_string(MOVIE_DIR_ENV_VAR, NULL);

    safe_assert(
        !make_movie || movie_dir != NULL,
        "If " MAKE_MOVIE_ENV_VAR " env var is set, " MOVIE_DIR_ENV_VAR
        " env variable must be set to create a movie\n"
    );

    return (KMeansArgs){
        .n_clusters = n_clusters,
        .input_file_path = input_file_path,
        .output_file_path = output_file_path,
        .max_iterations = K_MEANS_MAX_ITER,
        .tolerance = K_MEANS_TOL,
        .force_iterations = force_iterations,
        .make_movie = make_movie,
        .movie_dir = movie_dir,
    };
}

PointsCollection read_input_file(char* input_file_path, bool make_movie) {
    FILE* input_file = safe_fopen(input_file_path, "r");
    PointsCollection points = read_points_collection(input_file);
    fclose(input_file);

    safe_assert(
        !make_movie || points.dimensions == 2,
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

ClustersCollection create_clusters(
    size_t n_clusters, PointsCollection* points
) {
    ClustersCollection clusters =
        new_clusters_collection(points, n_clusters, NULL);
    init_centroids(&clusters, points);
    return clusters;
}

LoopData create_loop_data(double start_time) {
    return (LoopData){
        .maxsqshift = 0,
        .iteration = 0,
        .start_time = start_time,
    };
}

void reset_iteration(LoopData* loop) { loop->maxsqshift = 0; }

void print_iteration(FILE* stream, LoopData* loop) {
    fprintf(
        stream,
        "Iteration %3lu, maxsqshift = %" POINT_DISTANCE_OUT_FORMAT "\n",
        loop->iteration,
        loop->maxsqshift
    );
}

bool continue_loop(
    LoopData* loop,
    point_distance tolerance,
    size_t max_iterations,
    bool force_iterations
) {
    return (force_iterations || (loop->maxsqshift > tolerance * tolerance)) &&
           (loop->iteration <= max_iterations);
}

double finish_loop(FILE* stream, LoopData* loop, double end_time) {
    double elapsed = end_time - loop->start_time;
    fprintf(stream, "\nMain loop completed\n");
    fprintf(stream, "Elapsed seconds: %.3f\n\n", elapsed);
    return elapsed;
}

void write_output_file(
    KMeansArgs* args, ClustersCollection* clusters, PointsCollection* points
) {
    FILE* output_file = safe_fopen(args->output_file_path, "w");
    size_t dims = points->dimensions;
    size_t n_points = points->size;
    size_t n_centroids = clusters->centroids.size;

    fprintf(output_file, "# Data points: %lu\n", n_points);
    fprintf(output_file, "# Dimensions: %lu\n", dims);
    fprintf(output_file, "# Clusters: %lu\n", n_centroids);
    fprintf(output_file, "# Centroids:\n#\n");
    for (size_t i = 0; i < n_centroids; i++) {
        fprintf(output_file, "# %3lu :", i);
        for (size_t j = 0; j < dims; j++) {
            size_t idx = flat_index(i, j, dims);
            fprintf(
                output_file,
                " %" POINT_COORD_OUT_FORMAT,
                clusters->centroids.data[idx]
            );
        }
        fprintf(output_file, "\n");
    }
    fprintf(output_file, "#\n");
    for (size_t i = 0; i < n_points; i++) {
        for (size_t j = 0; j < dims; j++) {
            size_t idx = flat_index(i, j, dims);
            fprintf(
                output_file,
                "%" POINT_COORD_OUT_FORMAT " ",
                points->data[idx]
            );
        }
        fprintf(output_file, "%lu\n", clusters->cluster_of[i]);
    }

    fclose(output_file);
}
