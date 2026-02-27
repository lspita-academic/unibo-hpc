// Ludovico Maria Spitaleri 0001114169

#ifndef CLASSIFICATION_H
#define CLASSIFICATION_H

#include "clusters.h"
#include "points.h"

/**
 * Assign each point to a cluster.
 */
void classify_points(ClustersCollection* clusters);

/**
 * Update the centroids of each cluster to the barycenter of its points.
 * Returns the maximum shift in the centroids.
 */
point_distance update_centroids(ClustersCollection* clusters);

#endif  // CLASSIFICATION_H
