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

/*
 * Note on OpenMP
 *
 * From https://www.openmp.org/spec-html/5.0/openmpsu107.html:
 * A variable that is part of another variable, with the exception of array
 * elements, cannot appear in a reduction clause.
 *
 * For this reason, when a reduction must be performed on a struct field that is
 * a pointer, a function-local variable pointing to the same location is used.
 */

/*
 * Assign each point to the cluster with the nearest centroid.
 */
void classify_points(ClustersCollection* clusters, PointsCollection* points) {
    size_t dims = points->dimensions;
    size_t n_points = points->size;
    size_t n_centroids = clusters->centroids.size;
    size_t* counts = clusters->counts;

    // reset the assignments
#pragma omp single
    for (size_t i = 0; i < n_centroids; i++) {
        counts[i] = 0;
    }

#pragma omp for schedule(static) reduction(+ : counts[ : n_centroids])
    for (size_t i = 0; i < n_points; i++) {
        // get the index and squared distance of the nearest centroid
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

/*
 * Set the centroid of each cluster to the barycenter of its points.
 * Sets the out parameter `out_maxsqshift` to the maximum shift, i.e., the
 * maximum difference between the (squared) old and new position of all
 * centroids.
 */
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

    // initialize the centroids to zero
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
            // average the points in the cluster to get the new centroid
            point_scalar_mul(
                &new_centroids_data[idx], 1.0f / clusters->counts[i], dims
            );
        }

        // calculate the shift and store the max shift
        point_distance sqshift = points_squared_distance(
            &clusters->centroids.data[idx], &new_centroids_data[idx], dims
        );
        if (sqshift > *out_maxsqshift) {
            *out_maxsqshift = sqshift;
        }

        // copy the new centroid into the original clusters collection
        point_copy(
            &clusters->centroids.data[idx], &new_centroids_data[idx], dims
        );
    }
}

int main(int argc, char* argv[]) {
    // initialize all data
    init_random();
    KMeansArgs args = get_args(argc, argv);
    PointsCollection points =
        read_input_file(args.input_file_path, args.make_movie);
    print_inputs(stdout, &args, &points);
    ClustersCollection clusters = create_clusters(args.n_clusters, &points);
    PointsCollection new_centroids =
        new_points_collection(clusters.centroids.size, points.dimensions, NULL);

    // start the loop
    LoopData loop = create_loop_data(hpc_gettime());

    /*
     * Only one parallel region is used for the entire loop to avoid the
     * continuous creation and destruction of the thread pool.
     */
#pragma omp parallel default(none) \
    shared(clusters, points, loop, new_centroids, args, stdout)
    {
        do {
            /*
             * Wait for the threads still checking to continue the loop before
             * resetting the loop data.
             */
#pragma omp barrier

            /*
             * The reset of the iteration data can be moved after the
             * classification of points since it's not used in this function.
             * This allows to reuse the already needed single threaded region to
             * avoid adding an extra pragma.
             */
            classify_points(&clusters, &points);

            /*
             * `single` instead of `master` is used to ensure the presence of a
             * barrier at the end of the block, allowing synchronization between
             * the threads to ensure all points are classified before updating
             * the centroids.
             */
#pragma omp single
            {
                reset_iteration(&loop);  // reset of the iteration moved here
                if (args.make_movie) {
                    save_movie_iteration(
                        args.movie_dir, loop.iteration, &clusters, &points
                    );
                }
            }

            update_centroids(
                &clusters, &points, &new_centroids, &loop.maxsqshift
            );

            /*
             * `single` instead of `master` is used to ensure the presence of a
             * barrier at the end of the block, ensuring the loop data is
             * updated before checking to continue the loop.
             */
#pragma omp single
            {
                print_iteration(stdout, &loop);
                loop.iteration++;
            }
        } while (continue_loop(
            &loop, args.tolerance, args.max_iterations, args.force_iterations
        ));
    }
    // free the extra buffer
    free_points_collection(&new_centroids);

    // write the output
    finish_loop(stdout, &loop, hpc_gettime());
    write_output_file(&args, &clusters, &points);

    // free remaining data
    free_clusters_collection(&clusters);
    free_points_collection(&points);

    return EXIT_SUCCESS;
}
