// Ludovico Maria Spitaleri 0001114169

#include "clusters.h"

#include "memory.h"

ClustersCollection new_clusters_collection(
    PointsCollection* points, size_t n_clusters
) {
    PointsCollection centroids =
        new_points_collection(n_clusters, points->dimensions, NULL);

    size_t* cluster_of = safe_malloc(sizeof(*cluster_of) * points->size);
    size_t* counts = safe_malloc(sizeof(*counts) * n_clusters);

    return (ClustersCollection){.points = points,
                                .centroids = centroids,
                                .cluster_of = cluster_of,
                                .counts = counts};
}

void free_clusters_collection(ClustersCollection* clusters) {
    free_points_collection(&clusters->centroids);
    clusters->cluster_of = safe_free(clusters->cluster_of);
    clusters->counts = safe_free(clusters->counts);
}
