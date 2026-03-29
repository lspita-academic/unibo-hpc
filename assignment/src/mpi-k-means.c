// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

// hpc checks for mpi functions, so the mpi header must be included before
// clang-format off
#include <mpi.h>
#include <hpc.h>
// clang-format on
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "clusters.h"
#include "k-means.h"
#include "memory.h"
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
    int mpi_rank,
    ClustersCollection* master_clusters,
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

    /**
     * Raw point coord type is used instead of custom point mpi type to avoid
     * the creation of a custom reduce operation.
     *
     * From man pages of MPI_Reduce:
     * USE OF IN-PLACE OPTION
     * When the communicator is an intracommunicator, you can perform a reduce
     * operation in-place (the output buffer is used as the input buffer). Use
     * the variable MPI_IN_PLACE as the value of the root process sendbuf. In
     * this case, the input data is taken at the root from the receive buffer,
     * where it will be replaced by the output data.
     */
    MPI_Reduce(
        mpi_is_master(mpi_rank) ? MPI_IN_PLACE : new_centroids->data,
        new_centroids->data,
        clusters->centroids.size * dims,
        MPI_POINT_COORD,
        MPI_SUM,
        MPI_MASTER_RANK,
        MPI_DEFAULT_COMM
    );

    // The number of clusters is a lot smaller than the number of points, so the
    // overhead to split the work between the mpi processes makes the program
    // slower than the serial version for most of the cases.
    if (!mpi_is_master(mpi_rank)) {
        return;
    }

    for (size_t i = 0; i < master_clusters->size; i++) {
        size_t idx = flat_index(i, 0, dims);
        if (master_clusters->counts[i] == 0) {
            // cluster is empty, we simply copy the old centroid to the new one
            points_copy(
                &new_centroids->data[idx],
                &master_clusters->centroids.data[idx],
                dims
            );
        } else {
            // average the points in the cluster
            point_scalar_mul(
                &new_centroids->data[idx],
                1.0f / master_clusters->counts[i],
                dims
            );
        }

        // calculate the shift
        point_distance sqshift = points_distance(
            &master_clusters->centroids.data[idx],
            &new_centroids->data[idx],
            dims
        );
        if (sqshift > *out_maxsqshift) {
            *out_maxsqshift = sqshift;
        }

        // master_clusters and clusters centroids data point to the same
        // location in the master process
        points_copy(
            &master_clusters->centroids.data[idx],
            &new_centroids->data[idx],
            dims
        );
    }
}

typedef struct InputInfo {
    size_t total_points;
    size_t dimensions;
    size_t n_clusters;
} InputInfo;

MPI_Datatype mpi_input_info_type(void) {
    // https://www.mpich.org/static/docs/v3.1/www3/MPI_Type_create_struct.html
    // https://stackoverflow.com/a/9865041
    MPI_Datatype input_info_type;

    int n_blocks = 3;
    int block_lengths[] = {1, 1, 1};
    MPI_Aint displacements[] = {
        offsetof(InputInfo, total_points),
        offsetof(InputInfo, dimensions),
        offsetof(InputInfo, n_clusters)
    };
    MPI_Datatype types[] = {MPI_SIZE_T, MPI_SIZE_T, MPI_SIZE_T};

    MPI_Type_create_struct(
        n_blocks, block_lengths, displacements, types, &input_info_type
    );
    MPI_Type_commit(&input_info_type);
    return input_info_type;
}

int main(int argc, char* argv[]) {
    KMeansArgs master_args;
    PointsCollection master_points;
    ClustersCollection master_clusters;
    int* master_n_points = NULL;
    int* master_points_displacements = NULL;

    MPI_Init(&argc, &argv);
    int mpi_rank, mpi_nproc;
    MPI_Comm_rank(MPI_DEFAULT_COMM, &mpi_rank);
    MPI_Comm_size(MPI_DEFAULT_COMM, &mpi_nproc);

    InputInfo input_info;
    if (mpi_is_master(mpi_rank)) {
        init_random();
        master_args = get_args(argc, argv);
        master_points = read_input_file(
            master_args.input_file_path, master_args.make_movie
        );
        print_inputs(stdout, &master_args, &master_points);
        master_clusters =
            create_clusters(master_args.n_clusters, &master_points);

        input_info = (InputInfo){
            .total_points = master_points.size,
            .dimensions = master_points.dimensions,
            .n_clusters = master_clusters.size,
        };
        master_n_points = safe_malloc(sizeof(*master_n_points) * mpi_nproc);
        master_points_displacements =
            safe_malloc(sizeof(*master_points_displacements) * mpi_nproc);
    }

    MPI_Datatype input_info_type = mpi_input_info_type();
    MPI_Bcast(
        &input_info, 1, input_info_type, MPI_MASTER_RANK, MPI_DEFAULT_COMM
    );

    size_t splitted_points = (input_info.total_points / mpi_nproc);
    int n_points =
        splitted_points +
        (mpi_rank == mpi_nproc - 1 ? input_info.total_points % mpi_nproc : 0);
    int points_displacement = splitted_points * mpi_rank;

    MPI_Gather(
        &n_points,
        1,
        MPI_INT,
        master_n_points,
        1,
        MPI_INT,
        MPI_MASTER_RANK,
        MPI_DEFAULT_COMM
    );
    MPI_Gather(
        &points_displacement,
        1,
        MPI_INT,
        master_points_displacements,
        1,
        MPI_INT,
        MPI_MASTER_RANK,
        MPI_DEFAULT_COMM
    );

    MPI_Datatype point_type = mpi_point_type(input_info.dimensions);
    PointsCollection points =
        new_points_collection(n_points, input_info.dimensions, NULL);
    MPI_Scatterv(
        master_points.data,
        master_n_points,
        master_points_displacements,
        point_type,
        points.data,
        n_points,
        point_type,
        MPI_MASTER_RANK,
        MPI_DEFAULT_COMM
    );

    ClustersCollection clusters = new_clusters_collection(
        &points,
        input_info.n_clusters,
        mpi_is_master(mpi_rank) ? master_clusters.centroids.data : NULL
    );

    LoopData loop;
    if (mpi_is_master(mpi_rank)) {
        loop = create_loop_data(hpc_gettime());
    }
    int continue_flag;
    PointsCollection new_centroids = new_points_collection(
        input_info.n_clusters, input_info.dimensions, NULL
    );
    do {
        if (mpi_is_master(mpi_rank)) {
            reset_iteration(&loop);
        }
        MPI_Bcast(
            clusters.centroids.data,
            clusters.centroids.size,
            point_type,
            MPI_MASTER_RANK,
            MPI_DEFAULT_COMM
        );

        classify_points(&clusters);
        MPI_Reduce(
            clusters.counts,
            master_clusters.counts,
            input_info.n_clusters,
            MPI_SIZE_T,
            MPI_SUM,
            MPI_MASTER_RANK,
            MPI_DEFAULT_COMM
        );
        MPI_Gatherv(
            clusters.cluster_of,
            clusters.points->size,
            MPI_SIZE_T,
            master_clusters.cluster_of,
            master_n_points,
            master_points_displacements,
            MPI_SIZE_T,
            MPI_MASTER_RANK,
            MPI_DEFAULT_COMM
        );

        if (mpi_is_master(mpi_rank) && master_args.make_movie) {
            save_movie_iteration(
                master_args.movie_dir, &master_clusters, loop.iteration
            );
        }

        update_centroids(
            mpi_rank,
            &master_clusters,
            &clusters,
            &new_centroids,
            &loop.maxsqshift
        );
        if (mpi_is_master(mpi_rank)) {
            print_iteration(stdout, &loop);
            loop.iteration++;
            continue_flag = continue_loop(
                &loop, master_args.tolerance, master_args.max_iterations
            );
        }
        MPI_Bcast(
            &continue_flag, 1, MPI_INT, MPI_MASTER_RANK, MPI_DEFAULT_COMM
        );
    } while (continue_flag);

    if (mpi_is_master(mpi_rank)) {
        finish_loop(stdout, &loop, hpc_gettime());
        write_output_file(&master_args, &master_clusters);

        free_clusters_collection(&master_clusters);
        clusters.centroids.data =
            NULL;  // prevent double free of centroids data
        free_points_collection(&master_points);

        master_n_points = safe_free(master_n_points);
        master_points_displacements = safe_free(master_points_displacements);
    }

    free_points_collection(&new_centroids);
    free_clusters_collection(&clusters);
    free_points_collection(&points);

    MPI_Type_free(&point_type);
    MPI_Type_free(&input_info_type);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
