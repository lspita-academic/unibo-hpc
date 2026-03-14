// Ludovico Maria Spitaleri 0001114169

#include "demo.h"

#include <stddef.h>

#include "array.h"
#include "files.h"
#include "points.h"

#define DEMO_FILENAME_BUFF_SIZE 1024

void save_centroids(FILE* stream, ClustersCollection* clusters) {
    size_t dims = clusters->points->dimensions;

    for (size_t i = 0; i < clusters->size; i++) {
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

void save_points_and_clusters(FILE* stream, ClustersCollection* clusters) {
    size_t dims = clusters->points->dimensions;

    for (size_t i = 0; i < clusters->points->size; i++) {
        for (size_t j = 0; j < dims; j++) {
            size_t idx = flat_index(i, j, dims);
            fprintf(
                stream,
                "%" POINT_COORD_OUT_FORMAT " ",
                clusters->points->data[idx]
            );
        }
        fprintf(stream, "%lu\n", clusters->cluster_of[i]);
    }
}

void save_demo_iteration(char* dir, ClustersCollection* clusters, size_t iter) {
    char buff[DEMO_FILENAME_BUFF_SIZE];

    // save centroids
    snprintf(
        buff, sizeof(buff), "%s/" DEMO_CENTROIDS_FILENAME_FORMAT, dir, iter
    );
    FILE* centroids_file = safe_fopen(buff, "w");
    save_centroids(centroids_file, clusters);
    safe_fclose(centroids_file);

    // save points-clusters association
    snprintf(
        buff, sizeof(buff), "%s/" DEMO_CLUSTERS_FILENAME_FORMAT, dir, iter
    );
    FILE* clusters_file = safe_fopen(buff, "w");
    save_points_and_clusters(clusters_file, clusters);
    safe_fclose(clusters_file);
}
