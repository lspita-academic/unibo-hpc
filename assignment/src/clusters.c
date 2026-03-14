// Ludovico Maria Spitaleri 0001114169

#include "clusters.h"

#include <stddef.h>

#include "array.h"
#include "memory.h"
#include "points.h"
#include "random.h"

ClustersCollection new_clusters_collection(
    PointsCollection* points, size_t n_clusters
) {
    PointsCollection centroids =
        new_points_collection(n_clusters, points->dimensions, NULL);

    size_t* cluster_of = safe_malloc(sizeof(*cluster_of) * points->size);
    size_t* counts = safe_malloc(sizeof(*counts) * n_clusters);

    return (ClustersCollection){.size = n_clusters,
                                .points = points,
                                .centroids = centroids,
                                .cluster_of = cluster_of,
                                .counts = counts};
}

void free_clusters_collection(ClustersCollection* clusters) {
    free_points_collection(&clusters->centroids);
    clusters->cluster_of = safe_free(clusters->cluster_of);
    clusters->counts = safe_free(clusters->counts);
}

void print_clusters_collection(FILE* stream, ClustersCollection* clusters) {
    size_t dims = clusters->points->dimensions;

    fprintf(stream, "# Data points: %lu\n", clusters->points->size);
    fprintf(stream, "# Dimensions: %lu\n", dims);
    fprintf(stream, "# Clusters: %lu\n", clusters->size);
    fprintf(stream, "# Centroids:\n#\n");
    for (size_t i = 0; i < clusters->size; i++) {
        fprintf(stream, "# %3lu :", i);
        for (size_t j = 0; j < dims; j++) {
            size_t idx = flat_index(i, j, dims);
            fprintf(
                stream,
                " %" POINT_COORD_OUT_FORMAT,
                clusters->centroids.data[idx]
            );
        }
        fprintf(stream, "\n");
    }
    fprintf(stream, "#\n");
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

void init_centroids(ClustersCollection* clusters) {
    size_t select = clusters->size;
    size_t remaining = clusters->points->size;
    size_t dims = clusters->points->dimensions;

    for (size_t i = 0; (i < clusters->points->size) && (select > 0); i++) {
        if ((rand_int() % remaining) < select) {
            select--;
            /* Select point `i` as one of the centroids. */
            size_t select_idx = flat_index(select, 0, dims);
            size_t idx = flat_index(i, 0, dims);
            points_copy(
                &clusters->centroids.data[select_idx],
                &clusters->points->data[idx],
                dims
            );
        }
        remaining--;
    }
}
