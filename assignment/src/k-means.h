// Ludovico Maria Spitaleri 0001114169

#ifndef K_MEANS_H
#define K_MEANS_H

#include <stdbool.h>

#include "clusters.h"
#include "points.h"

/**
 * K-means clustering algorithm arguments.
 */
typedef struct KMeansArgs {
    size_t n_clusters;
    char* input_file_path;
    char* output_file_path;
    size_t max_iterations;
    point_distance tolerance;
    bool make_movie;
    char* movie_dir;
} KMeansArgs;

typedef struct LoopData {
    point_distance maxsqshift;
    size_t iteration;
    double start_time;
} LoopData;

/**
 * Get algorithm arguments.
 */
KMeansArgs get_args(int argc, char* argv[]);

/**
 * Read points from the input file.
 */
PointsCollection read_input_file(KMeansArgs* args);

/**
 * Print algorithm arguments.
 */
void print_inputs(FILE* stream, KMeansArgs* args, PointsCollection* points);

/**
 * Create and initialize clusters collection.
 */
ClustersCollection create_clusters(KMeansArgs* args, PointsCollection* points);

/**
 * Create and initialize loop data.
 */
LoopData create_loop_data(double start_time);

/**
 * Reset loop data for new iteration.
 */
void reset_iteration(LoopData* loop);

/**
 * Print iteration data.
 */
void print_iteration(FILE* stream, LoopData* loop);

/**
 * Check if the loop should continue.
 */
bool continue_loop(LoopData* loop, KMeansArgs* args);

/**
 * End loop and return the elapsed time.
 */
double finish_loop(FILE* stream, LoopData* loop, double end_time);

/**
 * Write algorithm results to the output file.
 */
void write_output_file(KMeansArgs* args, ClustersCollection* clusters);

#endif  // K_MEANS_H
