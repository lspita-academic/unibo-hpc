// Ludovico Maria Spitaleri 0001114169

#ifndef MOVIE_H
#define MOVIE_H

#include <stddef.h>

#include "clusters.h"

#define MOVIE_CENTROIDS_FILENAME_FORMAT "centroids_%03lu.txt"
#define MOVIE_CLUSTERS_FILENAME_FORMAT "clusters_%03lu.txt"

/**
 * Save iteration data for movie generation.
 */
void save_movie_iteration(
    char* dir,
    size_t iter,
    ClustersCollection* clusters,
    PointsCollection* points
);

#endif  // MOVIE_H
