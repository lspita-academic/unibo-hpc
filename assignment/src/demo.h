// Ludovico Maria Spitaleri 0001114169

#ifndef DEMO_H
#define DEMO_H

#include <stddef.h>

#include "clusters.h"

#ifndef DEMO_CENTROIDS_FILENAME_FORMAT
#define DEMO_CENTROIDS_FILENAME_FORMAT "centroids_%03lu.txt"
#endif

#ifndef DEMO_CLUSTERS_FILENAME_FORMAT
#define DEMO_CLUSTERS_FILENAME_FORMAT "clusters_%03lu.txt"
#endif

/**
 * Save iteration data for demo generation.
 */
void save_demo_iteration(char* dir, ClustersCollection* clusters, size_t iter);

#endif  // DEMO_H
