// Ludovico Maria Spitaleri 0001114169

#ifndef CLUSTERS_H
#define CLUSTERS_H

#include "points.h"

/**
 * Collection of clusters.
 * The `centroids` array contains the centroid of each cluster, and its size
 * indicates the number of clusters.
 * The `cluster_of` array stores the cluster index of each point.
 * The `counts` array stores the number of points in each cluster.
 */
typedef struct ClustersCollection {
    PointsCollection centroids;
    size_t* cluster_of;
    size_t* counts;
} ClustersCollection;

/**
 * Create a `ClustersCollection` from a `PointsCollection`.
 */
ClustersCollection new_clusters_collection(
    PointsCollection* points, size_t n_clusters, point_coord* points_data
);

/**
 * Free the memory allocated for the fields of a `ClustersCollection`.
 */
void free_clusters_collection(ClustersCollection* clusters);

/**
 * Initialize the centroids of a `ClustersCollection`.
 * The centroids are initialized randomly using Knuths' algorithm (as reported
 * in J. Bentley, "Programming Pearls", 2nd ed., Addison-Wesley, 2000, p. 126).
 */
void init_centroids(ClustersCollection* clusters, PointsCollection* points);

#endif  // CLUSTERS_H
