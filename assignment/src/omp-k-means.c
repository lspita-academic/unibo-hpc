// Ludovico Maria Spitaleri 0001114169

#include <stddef.h>
#include <stdio.h>

#include "array.h"
#include "k-means.h"
#include "points.h"

void omp_classify_points(ClustersCollection* clusters) {
    size_t dims = clusters->points->dimensions;

    // OpenMP reduction must be used on a direct variable, so clusters->counts
    // cannot be used
    size_t* counts = clusters->counts;

#pragma omp parallel default(none) shared(clusters, dims, counts)
    {
#pragma omp for schedule(static)
        for (size_t i = 0; i < clusters->size; i++) {
            counts[i] = 0;
        }

#pragma omp for schedule(static) reduction(+ : counts[ : clusters->size])
        for (size_t i = 0; i < clusters->points->size; i++) {
            // index and squared distance of the nearest centroid
            size_t nearest = clusters->size;
            point_coord mindist = POINT_COORD_MAX;
            for (size_t j = 0; j < clusters->size; j++) {
                size_t idx = flat_index(i, 0, dims);
                size_t jdx = flat_index(j, 0, dims);

                point_coord dist = points_distance(
                    &clusters->points->data[idx],
                    &clusters->centroids.data[jdx],
                    dims
                );
                if (dist < mindist) {
                    mindist = dist;
                    nearest = j;
                }
            }

            // assign the point to the nearest centroid
            clusters->cluster_of[i] = nearest;
            counts[nearest]++;
        }
    }
}

point_distance omp_update_centroids(ClustersCollection* clusters) {
    size_t dims = clusters->points->dimensions;

    PointsCollection new_centroids =
        new_points_collection(clusters->size, dims, NULL);

    // initialize centroids to zero
    for (size_t i = 0; i < clusters->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        zero_point(&new_centroids.data[idx], dims);
    }

    // sum all points in their respective cluster centroid
    for (size_t i = 0; i < clusters->points->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        size_t cluster_idx = flat_index(clusters->cluster_of[i], 0, dims);

        points_add(
            &new_centroids.data[cluster_idx], &clusters->points->data[idx], dims
        );
    }

    point_distance maxsqshift = 0.0f;
    for (size_t i = 0; i < clusters->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        if (clusters->counts[i] == 0) {
            // cluster is empty, we simply copy the old centroid to the new one
            points_copy(
                &new_centroids.data[idx], &clusters->centroids.data[idx], dims
            );
        } else {
            // average the points in the cluster
            point_scalar_mul(
                &new_centroids.data[idx], 1.0f / clusters->counts[i], dims
            );
        }

        // calculate the shift
        const point_distance sqshift = points_distance(
            &clusters->centroids.data[idx], &new_centroids.data[idx], dims
        );
        if (sqshift > maxsqshift) {
            maxsqshift = sqshift;
        }

        points_copy(
            &clusters->centroids.data[idx], &new_centroids.data[idx], dims
        );
    }

    free_points_collection(&new_centroids);
    return maxsqshift;
}

int main(int argc, char* argv[]) {
    return k_means(argc, argv, omp_classify_points, omp_update_centroids);
}
