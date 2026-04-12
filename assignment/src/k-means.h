// Ludovico Maria Spitaleri 0001114169

#ifndef K_MEANS_H
#define K_MEANS_H

#include <stdbool.h>

#include "clusters.h"
#include "points.h"

#define K_MEANS_MAX_ITER 100
#define K_MEANS_TOL 1e-5

/**
 * K-means clustering algorithm arguments.
 */
typedef struct KMeansArgs {
    size_t n_clusters;
    char* input_file_path;
    char* output_file_path;
    size_t max_iterations;
    point_distance tolerance;
    bool force_iterations;
    bool make_movie;
    char* movie_dir;
} KMeansArgs;

typedef struct LoopData {
    point_distance maxsqshift;
    size_t iteration;
    double start_time;
} LoopData;

/**
 * Get the algorithm arguments.
 */
KMeansArgs get_args(int argc, char* argv[]);

/**
 * Read points from the input file.
 */
PointsCollection read_input_file(char* input_file_path, bool make_movie);

/**
 * Print the algorithm inputs.
 */
void print_inputs(FILE* stream, KMeansArgs* args, PointsCollection* points);

/**
 * Create and initialize a clusters collection.
 */
ClustersCollection create_clusters(size_t n_clusters, PointsCollection* points);

/**
 * Create and initialize the loop data.
 */
LoopData create_loop_data(double start_time);

/**
 * Reset the loop data for new iteration.
 */
void reset_iteration(LoopData* loop);

/**
 * Print the current iteration loop data.
 */
void print_iteration(FILE* stream, LoopData* loop);

/**
 * Check if the loop should continue.
 * If `force_iterations` is true, the tolerance is ignored and all
 * `K_MEANS_MAX_ITER` iterations will be done.
 */
bool continue_loop(
    LoopData* loop,
    point_distance tolerance,
    size_t max_iterations,
    bool force_iterations
);

/**
 * End the loop and return the elapsed time.
 */
double finish_loop(FILE* stream, LoopData* loop, double end_time);

/**
 * Write the algorithm results to the output file.
 */
void write_output_file(
    KMeansArgs* args, ClustersCollection* clusters, PointsCollection* points
);

#endif  // K_MEANS_H
