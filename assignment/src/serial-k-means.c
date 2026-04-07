// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

#include <hpc.h>
#include <stdlib.h>

#include "array.h"
#include "k-means.h"
#include "movie.h"
#include "points.h"
#include "random.h"

void classify_points(ClustersCollection* clusters) {
    size_t dims = clusters->points->dimensions;

    for (size_t i = 0; i < clusters->size; i++) {
        clusters->counts[i] = 0;
    }

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
        clusters->counts[nearest]++;
    }
}

void update_centroids(
    ClustersCollection* clusters,
    PointsCollection* new_centroids,
    point_distance* out_maxsqshift
) {
    size_t dims = clusters->points->dimensions;

    // initialize centroids to zero
    for (size_t i = 0; i < clusters->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        zero_point(&new_centroids->data[idx], dims);
    }

    // sum all points in their respective cluster centroid
    for (size_t i = 0; i < clusters->points->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        size_t cluster_idx = flat_index(clusters->cluster_of[i], 0, dims);

        points_add(
            &new_centroids->data[cluster_idx],
            &clusters->points->data[idx],
            dims
        );
    }

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
    PointsCollection points =
        read_input_file(args.input_file_path, args.make_movie);
    print_inputs(stdout, &args, &points);

    ClustersCollection clusters = create_clusters(args.n_clusters, &points);

    PointsCollection new_centroids =
        new_points_collection(clusters.size, clusters.points->dimensions, NULL);
    LoopData loop = create_loop_data(hpc_gettime());
    do {
        reset_iteration(&loop);
        classify_points(&clusters);
        if (args.make_movie) {
            save_movie_iteration(args.movie_dir, &clusters, loop.iteration);
        }
        update_centroids(&clusters, &new_centroids, &loop.maxsqshift);
        print_iteration(stdout, &loop);
        loop.iteration++;
    } while (continue_loop(
        &loop, args.tolerance, args.max_iterations, args.force_iterations
    ));
    free_points_collection(&new_centroids);

    finish_loop(stdout, &loop, hpc_gettime());
    write_output_file(&args, &clusters);

    free_clusters_collection(&clusters);
    free_points_collection(&points);

    return EXIT_SUCCESS;
}
