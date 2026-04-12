// Ludovico Maria Spitaleri 0001114169

#include "clusters.h"

#include <stddef.h>

#include "array.h"
#include "memory.h"
#include "points.h"
#include "random.h"

ClustersCollection new_clusters_collection(
    PointsCollection* points, size_t n_clusters, point_coord* centroids_data
) {
    PointsCollection centroids =
        new_points_collection(n_clusters, points->dimensions, centroids_data);

    size_t* cluster_of = safe_malloc(sizeof(*cluster_of) * points->size);
    size_t* counts = safe_malloc(sizeof(*counts) * n_clusters);

    return (ClustersCollection){
        .centroids = centroids,
        .cluster_of = cluster_of,
        .counts = counts,
    };
}

void free_clusters_collection(ClustersCollection* clusters) {
    free_points_collection(&clusters->centroids);
    clusters->cluster_of = safe_free(clusters->cluster_of);
    clusters->counts = safe_free(clusters->counts);
}

void init_centroids(ClustersCollection* clusters, PointsCollection* points) {
    size_t select = clusters->centroids.size;
    size_t dims = points->dimensions;
    size_t remaining = points->size;

    for (size_t i = 0; (i < points->size) && (select > 0); i++) {
        if ((rand_int() % remaining) < select) {
            select--;
            /* Select point `i` as one of the centroids. */
            size_t select_idx = flat_index(select, 0, dims);
            size_t idx = flat_index(i, 0, dims);
            point_copy(
                &clusters->centroids.data[select_idx],
                &points->data[idx],
                dims
            );
        }
        remaining--;
    }
}
