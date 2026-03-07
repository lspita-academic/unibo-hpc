// Ludovico Maria Spitaleri 0001114169

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "demo.h"
#include "k-means.h"
#include "points.h"
#include "random.h"

void classify_points(ClustersCollection* clusters) {
    size_t dims = clusters->points->dimensions;

    // OpenMP reduction must be used on a direct variable, so clusters->counts
    // cannot be used
    size_t* counts = clusters->counts;

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

void update_centroids(
    ClustersCollection* clusters,
    PointsCollection* new_centroids,
    point_distance* out_maxsqshift
) {
    size_t dims = clusters->points->dimensions;
    point_coord* new_centroids_data = new_centroids->data;

    // initialize centroids to zero
#pragma omp for schedule(static)
    for (size_t i = 0; i < clusters->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        zero_point(&new_centroids_data[idx], dims);
    }

    // sum all points in their respective cluster centroid
#pragma omp for schedule(static) \
    reduction(+ : new_centroids_data[ : clusters->size])
    for (size_t i = 0; i < clusters->points->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        size_t cluster_idx = flat_index(clusters->cluster_of[i], 0, dims);

        points_add(
            &new_centroids_data[cluster_idx], &clusters->points->data[idx], dims
        );
    }

#pragma omp for schedule(static) reduction(max : out_maxsqshift[0])
    for (size_t i = 0; i < clusters->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        if (clusters->counts[i] == 0) {
            // cluster is empty, we simply copy the old centroid to the new one
            points_copy(
                &new_centroids->data[idx], &clusters->centroids.data[idx], dims
            );
        } else {
            // average the points in the cluster
            point_scalar_mul(
                &new_centroids->data[idx], 1.0f / clusters->counts[i], dims
            );
        }

        // calculate the shift
        point_distance sqshift = points_distance(
            &clusters->centroids.data[idx], &new_centroids->data[idx], dims
        );
        if (sqshift > *out_maxsqshift) {
            *out_maxsqshift = sqshift;
        }

        points_copy(
            &clusters->centroids.data[idx], &new_centroids->data[idx], dims
        );
    }
}

int main(int argc, char* argv[]) {
    init_random();
    KMeansArgs args = get_args(argc, argv);
    PointsCollection points = read_input_file(&args);
    print_inputs(stdout, &args, &points);

    ClustersCollection clusters = create_clusters(&args, &points);

    LoopData loop = create_loop_data();
    PointsCollection new_centroids =
        new_points_collection(clusters.size, clusters.points->dimensions, NULL);
#pragma omp parallel default(none) \
    shared(clusters, loop, new_centroids, args, stdout)
    {
        do {
/* Threads must wait the one updating the loop data between iterations */
#pragma omp barrier
#pragma omp single
            {
                reset_iteration(&loop);
            }
            classify_points(&clusters);
#pragma omp single
            {
                if (args.make_movie) {
                    save_demo_iteration(
                        args.demo_dir, &clusters, loop.iteration
                    );
                }
                update_centroids(&clusters, &new_centroids, &loop.maxsqshift);
            }
#pragma omp single
            {
                print_iteration(stdout, &loop);
                loop.iteration++;
            }
        } while (continue_loop(&loop, &args));
    }
    free_points_collection(&new_centroids);

    finish_loop(stdout, &loop);
    write_output_file(&args, &clusters);

    free_clusters_collection(&clusters);
    free_points_collection(&points);

    return EXIT_SUCCESS;
}
