// Ludovico Maria Spitaleri 0001114169

#include "movie.h"

#include <stddef.h>

#include "array.h"
#include "files.h"
#include "points.h"

#define MOVIE_FILENAME_BUFF_SIZE 1024

/**
 * Save the centroids data to the output file.
 */
void save_centroids(
    FILE* stream, ClustersCollection* clusters
) {
    size_t dims = clusters->centroids.dimensions;
    size_t n_centroids = clusters->centroids.size;

    for (size_t i = 0; i < n_centroids; i++) {
        for (size_t j = 0; j < dims; j++) {
            size_t idx = flat_index(i, j, dims);
            fprintf(
                stream,
                "%" POINT_COORD_OUT_FORMAT " ",
                clusters->centroids.data[idx]
            );
        }
        fputc('\n', stream);
    }
}

/**
 * Save the points and corresponding cluster to the output file.
 */
void save_points_and_clusters(
    FILE* stream, ClustersCollection* clusters, PointsCollection* points
) {
    size_t dims = points->dimensions;
    size_t n_points = points->size;

    for (size_t i = 0; i < n_points; i++) {
        for (size_t j = 0; j < dims; j++) {
            size_t idx = flat_index(i, j, dims);
            fprintf(stream, "%" POINT_COORD_OUT_FORMAT " ", points->data[idx]);
        }
        fprintf(stream, "%lu\n", clusters->cluster_of[i]);
    }
}

void save_movie_iteration(
    char* dir,
    size_t iter,
    ClustersCollection* clusters,
    PointsCollection* points
) {
    char buff[MOVIE_FILENAME_BUFF_SIZE];

    // save centroids
    snprintf(
        buff, sizeof(buff), "%s/" MOVIE_CENTROIDS_FILENAME_FORMAT, dir, iter
    );
    FILE* centroids_file = safe_fopen(buff, "w");
    save_centroids(centroids_file, clusters);
    safe_fclose(centroids_file);

    // save points-clusters association
    snprintf(
        buff, sizeof(buff), "%s/" MOVIE_CLUSTERS_FILENAME_FORMAT, dir, iter
    );
    FILE* clusters_file = safe_fopen(buff, "w");
    save_points_and_clusters(clusters_file, clusters, points);
    safe_fclose(clusters_file);
}
