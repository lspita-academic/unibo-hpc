// Ludovico Maria Spitaleri 0001114169

#ifndef CLUSTERS_H
#define CLUSTERS_H

#include "points.h"

/**
 * Collection of `size` clusters related to the `points` collection.
 * The centroid of each cluster is stored in the `centroids` array.
 * The `cluster_of` array stores the cluster index of each point.
 * The `counts` array stores the number of points in each cluster.
 */
typedef struct ClustersCollection {
    PointsCollection* points;
    PointsCollection centroids;
    size_t size;
    size_t* cluster_of;
    size_t* counts;
} ClustersCollection;

/**
 * Create a `ClustersCollection` from a `PointsCollection`.
 */
ClustersCollection new_clusters_collection(
    PointsCollection* points, size_t n_clusters
);

/**
 * Free the memory allocated for the fields of a `ClustersCollection`.
 * Associated point collection will not be freed.
 */
void free_clusters_collection(ClustersCollection* clusters);

/**
 * Initialize the centroids of a `ClustersCollection`.
 * The centroids are initialized randomly using Knuths' algorithm (as reported
 * in J. Bentley, "Programming Pearls", 2nd ed., Addison-Wesley, 2000, p. 126).
 *
 * DO NOT PARALLELIZE THIS FUNCTION: `rand` is not thread-safe.
 */
void init_centroids(ClustersCollection* clusters);

#endif  // CLUSTERS_H
