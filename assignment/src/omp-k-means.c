// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

#include <hpc.h>
#include <omp.h>
#include <stdlib.h>

#include "array.h"
#include "k-means.h"
#include "movie.h"
#include "points.h"
#include "random.h"

void classify_points(ClustersCollection* clusters, PointsCollection* points) {
    size_t dims = points->dimensions;
    size_t n_points = points->size;
    size_t n_centroids = clusters->centroids.size;

    // OpenMP reduction must be used on a direct variable, so clusters->counts
    // cannot be used
    size_t* counts = clusters->counts;

// The number of clusters is a lot smaller than the number of points, so the
// overhead to split the work between the openmp threads makes the program
// slower than the serial version for most of the cases.
#pragma omp single
    for (size_t i = 0; i < n_centroids; i++) {
        counts[i] = 0;
    }

#pragma omp for schedule(static) reduction(+ : counts[ : n_centroids])
    for (size_t i = 0; i < n_points; i++) {
        // index and squared distance of the nearest centroid
        size_t nearest = n_centroids;
        point_coord mindist = POINT_COORD_MAX;
        for (size_t j = 0; j < n_centroids; j++) {
            size_t idx = flat_index(i, 0, dims);
            size_t jdx = flat_index(j, 0, dims);

            point_coord dist = points_squared_distance(
                &points->data[idx], &clusters->centroids.data[jdx], dims
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
    PointsCollection* points,
    PointsCollection* new_centroids,
    point_distance* out_maxsqshift
) {
    size_t dims = points->dimensions;
    size_t n_points = points->size;
    size_t n_centroids = clusters->centroids.size;
    point_coord* new_centroids_data = new_centroids->data;

// initialize centroids to zero
#pragma omp single
    for (size_t i = 0; i < n_centroids; i++) {
        size_t idx = flat_index(i, 0, dims);
        zero_point(&new_centroids_data[idx], dims);
    }

// sum all points in their respective cluster centroid
#pragma omp for schedule(static) \
    reduction(+ : new_centroids_data[ : n_centroids * dims])
    for (size_t i = 0; i < n_points; i++) {
        size_t idx = flat_index(i, 0, dims);
        size_t cluster_idx = flat_index(clusters->cluster_of[i], 0, dims);
        points_add(&new_centroids_data[cluster_idx], &points->data[idx], dims);
    }

#pragma omp single
    for (size_t i = 0; i < n_centroids; i++) {
        size_t idx = flat_index(i, 0, dims);
        if (clusters->counts[i] == 0) {
            // cluster is empty, we simply copy the old centroid to the new one
            point_copy(
                &new_centroids_data[idx], &clusters->centroids.data[idx], dims
            );
        } else {
            // average the points in the cluster
            point_scalar_mul(
                &new_centroids_data[idx], 1.0f / clusters->counts[i], dims
            );
        }

        // calculate the shift
        point_distance sqshift = points_squared_distance(
            &clusters->centroids.data[idx], &new_centroids_data[idx], dims
        );
        if (sqshift > *out_maxsqshift) {
            *out_maxsqshift = sqshift;
        }

        point_copy(
            &clusters->centroids.data[idx], &new_centroids_data[idx], dims
        );
    }
}

int main(int argc, char* argv[]) {
    init_random();
    KMeansArgs args = get_args(argc, argv);
    PointsCollection points =
        read_input_file(args.input_file_path, args.make_movie);
    print_inputs(stdout, &args, &points);

    ClustersCollection clusters = create_clusters(args.n_clusters, &points);

    PointsCollection new_centroids =
        new_points_collection(clusters.centroids.size, points.dimensions, NULL);
    LoopData loop = create_loop_data(hpc_gettime());
#pragma omp parallel default(none) \
    shared(clusters, points, loop, new_centroids, args, stdout)
    {
        do {
/* Threads must wait the ones still checking to continue the loop before
 * resetting */
#pragma omp barrier
#pragma omp single
            {
                reset_iteration(&loop);
            }

            classify_points(&clusters, &points);

#pragma omp single
            {
                if (args.make_movie) {
                    save_movie_iteration(
                        args.movie_dir, loop.iteration, &clusters, &points
                    );
                }
            }

            update_centroids(
                &clusters, &points, &new_centroids, &loop.maxsqshift
            );

#pragma omp single
            {
                print_iteration(stdout, &loop);
                loop.iteration++;
            }
        } while (continue_loop(
            &loop, args.tolerance, args.max_iterations, args.force_iterations
        ));
    }
    free_points_collection(&new_centroids);

    finish_loop(stdout, &loop, hpc_gettime());
    write_output_file(&args, &clusters, &points);

    free_clusters_collection(&clusters);
    free_points_collection(&points);

    return EXIT_SUCCESS;
}
