// Ludovico Maria Spitaleri 0001114169

#ifndef CLUSTERS_H
#define CLUSTERS_H

#include "points.h"

/**
 * Array of `size` clusters related to the `points` array.
 * The centroid of each cluster is stored in the `centroids` array.
 * The `cluster_of` array stores the cluster index of each point.
 * The `counts` array stores the number of points in each cluster.
 */
typedef struct ClustersArray {
    PointsArray* points;
    PointsArray centroids;
    size_t size;
    size_t* cluster_of;
    size_t* counts;
} ClustersArray;

/**
 * Create a `ClustersArray` from a `PointsArray`.
 */
ClustersArray new_clusters_array(PointsArray* points, size_t n_clusters);

/**
 * Free the memory allocated for the fields of a `ClustersArray`.
 */
void free_clusters_array(ClustersArray* clusters);

#endif  // CLUSTERS_H
