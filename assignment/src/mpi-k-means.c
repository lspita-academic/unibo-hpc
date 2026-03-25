// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#include <stddef.h>

#include "clusters.h"
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

#include <hpc.h>
#include <mpi.h>
#include <stdlib.h>

#include "array.h"
#include "k-means.h"
#include "movie.h"
#include "mpi-points.h"
#include "mpi-utils.h"
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

typedef struct BroadcastData {
    size_t points_size;
    size_t dimensions;
    size_t n_clusters;
    point_coord* centroids_data;
} BroadcastData;

MPI_Datatype mpi_broadcast_type(BroadcastData *broadcast_data) {
    // TODO: broadcast data mpi datatype (1 block per parameter?)
    int n_blocks = 2; // size_t parameters +
}

int main(int argc, char* argv[]) {
    KMeansArgs master_args;
    PointsCollection master_points;
    ClustersCollection master_clusters;

    MPI_Init(&argc, &argv);

    int mpi_rank, mpi_nproc;
    MPI_Comm_rank(MPI_DEFAULT_COMM, &mpi_rank);
    MPI_Comm_size(MPI_DEFAULT_COMM, &mpi_nproc);

    size_t args_buff[3];
    MPI_Datatype mpi_args_type;
    MPI_Type_contiguous(3, MPI_SIZE_T, &mpi_args_type);
    MPI_Type_commit(&mpi_args_type);

    if (mpi_is_master(mpi_rank)) {
        init_random();
        master_args = get_args(argc, argv);
        master_points = read_input_file(
            master_args.input_file_path, master_args.make_movie
        );
        print_inputs(stdout, &master_args, &master_points);
        master_clusters =
            create_clusters(master_args.n_clusters, &master_points);

        args_buff[0] =
            (master_points.size / mpi_nproc) +
            (mpi_is_master(mpi_rank) ? master_points.size % mpi_nproc : 0);
        args_buff[1] = master_points.dimensions;
        args_buff[2] = master_clusters.size;
    }

    MPI_Bcast(args_buff, 1, mpi_args_type, MPI_MASTER_RANK, MPI_DEFAULT_COMM);
    size_t points_size = args_buff[0];
    size_t dimensions = args_buff[1];
    size_t n_clusters = args_buff[2];

    PointsCollection points =
        new_points_collection(points_size, dimensions, NULL);
    ClustersCollection clusters = new_clusters_collection(&points, n_clusters);
    PointsCollection new_centroids =
        new_points_collection(n_clusters, dimensions, NULL);

    LoopData loop = create_loop_data(hpc_gettime());
    do {
        reset_iteration(&loop);
        classify_points(&master_clusters);
        if (mpi_is_master(mpi_rank) && master_args.make_movie) {
            save_movie_iteration(
                master_args.movie_dir, &master_clusters, loop.iteration
            );
        }
        update_centroids(&master_clusters, &new_centroids, &loop.maxsqshift);
        print_iteration(stdout, &loop);
        loop.iteration++;
    } while (
        continue_loop(&loop, master_args.tolerance, master_args.max_iterations)
    );
    free_points_collection(&new_centroids);
    free_clusters_collection(&clusters);
    free_points_collection(&points);

    if (mpi_is_master(mpi_rank)) {
        finish_loop(stdout, &loop, hpc_gettime());
        write_output_file(&master_args, &master_clusters);

        free_clusters_collection(&master_clusters);
        free_points_collection(&master_points);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
