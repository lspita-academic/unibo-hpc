// Ludovico Maria Spitaleri 0001114169

#ifndef K_MEANS_H
#define K_MEANS_H

#include "clusters.h"

typedef void (*classify_points_fn)(ClustersCollection*);

typedef point_distance (*update_centroids_fn)(ClustersCollection*);

int k_means(
    int argc,
    char* argv[],
    classify_points_fn classify,
    update_centroids_fn update_centroids
);

#endif  // K_MEANS_H
