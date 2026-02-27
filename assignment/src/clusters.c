// Ludovico Maria Spitaleri 0001114169

#include "clusters.h"

#include "memory.h"

ClustersArray new_clusters_array(PointsArray* points, size_t n_clusters) {
    PointsArray centroids =
        new_points_array(n_clusters, points->dimensions, NULL);

    size_t* cluster_of = safe_malloc(sizeof(*cluster_of) * points->size);
    size_t* counts = safe_malloc(sizeof(*counts) * n_clusters);

    return (ClustersArray){.points = points,
                           .centroids = centroids,
                           .cluster_of = cluster_of,
                           .counts = counts};
}

void free_clusters_array(ClustersArray* clusters) {
    free_points_array(&clusters->centroids);
    clusters->cluster_of = safe_free(clusters->cluster_of);
    clusters->counts = safe_free(clusters->counts);
}
