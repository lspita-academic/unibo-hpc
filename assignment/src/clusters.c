// Ludovico Maria Spitaleri 0001114169

#include "clusters.h"

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

void init_centroids(ClustersCollection* clusters) {
    size_t select = clusters->size;
    size_t remaining = clusters->points->size;
    for (int i = 0; (i < clusters->points->size) && (select > 0); i++) {
        if ((rand_int() % remaining) < select) {
            select--;
            /* Select point `i` as one of the centroids. */
            copy_point(
                &clusters->centroids.data[i],
                &clusters->points->data[i],
                clusters->points->dimensions
            );
        }
        remaining--;
    }
}
