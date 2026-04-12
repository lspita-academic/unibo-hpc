// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used.
 */
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

/*
 * Clang format must be disabled here because hpc.h checks for mpi functions to
 * decide what to compile, so the mpi header must be included before regardless
 * of the include order imposed by the autoformatter.
 */
// clang-format off
#include <mpi.h>

/*
 * hpc.h checks for a macro named `MPI_Init` to determine if mpi is included,
 * but `MPI_Init` is a function, so a def/undef hack is used.
 */
#define MPI_Init
#include <hpc.h>
#undef MPI_Init

// clang-format on
#include <stddef.h>
#include <stdlib.h>

#include "abort.h"
#include "array.h"
#include "clusters.h"
#include "k-means.h"
#include "memory.h"
#include "movie.h"
#include "mpi-abort.h"
#include "mpi-points.h"
#include "mpi-utils.h"
#include "points.h"
#include "random.h"

/*
 * Assign each point to the cluster with the nearest centroid.
 */
void classify_points(ClustersCollection* clusters, PointsCollection* points) {
    size_t dims = points->dimensions;
    size_t n_points = points->size;
    size_t n_centroids = clusters->centroids.size;

    // reset the assignments
    for (size_t i = 0; i < n_centroids; i++) {
        clusters->counts[i] = 0;
    }

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
        clusters->counts[nearest]++;
    }
}

/*
 * Set the centroid of each cluster to the barycenter of its points.
 * Sets the out parameter `out_maxsqshift` to the maximum shift, i.e., the
 * maximum difference between the (squared) old and new position of all
 * centroids.
 * The out parameters are set only for the master process.
 */
void update_centroids(
    int mpi_rank,
    ClustersCollection* master_clusters,
    ClustersCollection* clusters,
    PointsCollection* points,
    PointsCollection* new_centroids,
    point_distance* out_maxsqshift
) {
    size_t dims = points->dimensions;
    size_t n_points = points->size;
    size_t n_centroids = clusters->centroids.size;

    // initialize the centroids to zero
    for (size_t i = 0; i < n_centroids; i++) {
        size_t idx = flat_index(i, 0, dims);
        zero_point(&new_centroids->data[idx], dims);
    }

    // sum all points in their respective cluster centroid
    for (size_t i = 0; i < n_points; i++) {
        size_t idx = flat_index(i, 0, dims);
        size_t cluster_idx = flat_index(clusters->cluster_of[i], 0, dims);
        points_add(&new_centroids->data[cluster_idx], &points->data[idx], dims);
    }

    /**
     * Gather all data from the mpi processes to continue with the master only.
     *
     * Raw point coord type is used instead of custom point mpi type to avoid
     * the need of a custom reduce operation.
     * Also, in-place reduction is used to avoid the need of an extra
     * `master_new_centroids` buffer.
     *
     * From MPI_REDUCE(3) man pages:
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
        n_centroids * dims,
        MPI_POINT_COORD,
        MPI_SUM,
        MPI_MASTER_RANK,
        MPI_DEFAULT_COMM
    );
    if (!mpi_is_master(mpi_rank)) {
        return;
    }

    for (size_t i = 0; i < n_centroids; i++) {
        size_t idx = flat_index(i, 0, dims);
        if (master_clusters->counts[i] == 0) {
            // cluster is empty, copy the old centroid to the new one
            point_copy(
                &new_centroids->data[idx],
                &master_clusters->centroids.data[idx],
                dims
            );
        } else {
            // average the points in the cluster to get the new centroid
            point_scalar_mul(
                &new_centroids->data[idx],
                1.0f / master_clusters->counts[i],
                dims
            );
        }

        // calculate the shift and store the max shift
        point_distance sqshift = points_squared_distance(
            &master_clusters->centroids.data[idx],
            &new_centroids->data[idx],
            dims
        );
        if (sqshift > *out_maxsqshift) {
            *out_maxsqshift = sqshift;
        }

        // copy the new centroid into the original clusters collection
        point_copy(
            &master_clusters->centroids.data[idx],
            &new_centroids->data[idx],
            dims
        );
    }
}

/*
 * Input data parameters sent to all mpi processes.
 */
typedef struct InputInfo {
    size_t total_points;
    size_t dimensions;
    size_t n_centroids;
} InputInfo;

/*
 * Create an mpi datatype corresponding to `InputInfo`.
 */
MPI_Datatype mpi_input_info_type(void) {
    /*
     * MPI_Type_create_struct documentation:
     * https://www.mpich.org/static/docs/v3.1/www3/MPI_Type_create_struct.html
     *
     * Stackoverflow response used as base:
     * https://stackoverflow.com/a/9865041
     *
     * From offsetof(3) man pages:
     * The macro offsetof() returns the offset of the field member from the
     * start of the structure type.
     * This  macro  is  useful because the sizes of the fields that compose a
     * structure can vary across implementations, and compilers may insert
     * different numbers of padding bytes between fields.  Conse‐ quently, an
     * element's offset is not necessarily given by the sum of the sizes of the
     * previous elements.
     * A compiler error will result if member is not aligned to a byte boundary
     * (i.e., it is a bit field).
     */
    MPI_Datatype input_info_type;

    int n_blocks = 3;
    int block_lengths[] = {1, 1, 1};
    MPI_Aint displacements[] = {
        offsetof(InputInfo, total_points),
        offsetof(InputInfo, dimensions),
        offsetof(InputInfo, n_centroids)
    };
    MPI_Datatype types[] = {MPI_SIZE_T, MPI_SIZE_T, MPI_SIZE_T};

    MPI_Type_create_struct(
        n_blocks, block_lengths, displacements, types, &input_info_type
    );
    MPI_Type_commit(&input_info_type);
    return input_info_type;
}

void calculate_input_portion(
    int mpi_rank,
    int mpi_nproc,
    size_t total_points,
    int* out_n_points,
    int* out_points_displacement
) {
    size_t splitted_points = (total_points / mpi_nproc);
    int n_points = splitted_points +
                   (mpi_rank == mpi_nproc - 1 ? total_points % mpi_nproc : 0);
    int points_displacement = splitted_points * mpi_rank;

    *out_n_points = n_points;
    *out_points_displacement = points_displacement;
}

int main(int argc, char* argv[]) {
    // initialize all data
    KMeansArgs master_args;
    PointsCollection master_points;
    ClustersCollection master_clusters;
    int* master_n_points = NULL;
    int* master_points_displacements = NULL;

    // set the exit function to an mpi variant that ensures proper abortion.
    set_exit_function(mpi_safe_exit);

    MPI_Init(&argc, &argv);
    int mpi_rank, mpi_nproc;
    MPI_Comm_rank(MPI_DEFAULT_COMM, &mpi_rank);
    MPI_Comm_size(MPI_DEFAULT_COMM, &mpi_nproc);

    InputInfo input_info;
    if (mpi_is_master(mpi_rank)) {
        /*
         * Since a fixed random seed is used, each process could initialize its
         * centroids and get the exact same values.
         * To avoid making the algorithm dependent on this behaviour, true
         * randomness is assumed, and only the master initializes all data and
         * shares it with the other mpi processes.
         */
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
            .n_centroids = master_clusters.centroids.size,
        };

        /*
         * The master needs to know the points portion of each mpi process.
         * Since the calculation is very simple, it is also repeated by
         * each process for its portion instead of making the master distribute
         * the result.
         */
        master_n_points = safe_malloc(sizeof(*master_n_points) * mpi_nproc);
        master_points_displacements =
            safe_malloc(sizeof(*master_points_displacements) * mpi_nproc);
        for (int i = 0; i < mpi_nproc; i++) {
            calculate_input_portion(
                i,
                mpi_nproc,
                input_info.total_points,
                &master_n_points[i],
                &master_points_displacements[i]
            );
        }
    }

    MPI_Datatype input_info_type = mpi_input_info_type();
    MPI_Bcast(
        &input_info, 1, input_info_type, MPI_MASTER_RANK, MPI_DEFAULT_COMM
    );

    int n_points;
    int points_displacement;
    calculate_input_portion(
        mpi_rank,
        mpi_nproc,
        input_info.total_points,
        &n_points,
        &points_displacement
    );

    // scatter the points collection across all mpi processes
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
        input_info.n_centroids,
        /*
         * the master process reuses the original centroids buffer to avoid
         * extra data copy operations.
         */
        mpi_is_master(mpi_rank) ? master_clusters.centroids.data : NULL
    );
    PointsCollection new_centroids = new_points_collection(
        input_info.n_centroids, input_info.dimensions, NULL
    );

    // start the loop
    LoopData loop;
    int continue_flag;
    if (mpi_is_master(mpi_rank)) {
        loop = create_loop_data(hpc_gettime());
    }
    do {
        if (mpi_is_master(mpi_rank)) {
            reset_iteration(&loop);
        }
        /*
         * Share the current centroids with all mpi processes.
         * Since the centroids data buffer of the master process points to the
         * same location of the original centroids buffer, it already contains
         * the data that needs to be broadcasted.
         */
        MPI_Bcast(
            clusters.centroids.data,
            clusters.centroids.size,
            point_type,
            MPI_MASTER_RANK,
            MPI_DEFAULT_COMM
        );

        classify_points(&clusters, &points);

        // gather the classification results into the master process
        MPI_Reduce(
            clusters.counts,
            master_clusters.counts,
            input_info.n_centroids,
            MPI_SIZE_T,
            MPI_SUM,
            MPI_MASTER_RANK,
            MPI_DEFAULT_COMM
        );
        MPI_Gatherv(
            clusters.cluster_of,
            points.size,
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
                master_args.movie_dir,
                loop.iteration,
                &master_clusters,
                &master_points
            );
        }

        update_centroids(
            mpi_rank,
            &master_clusters,
            &clusters,
            &points,
            &new_centroids,
            &loop.maxsqshift
        );
        if (mpi_is_master(mpi_rank)) {
            print_iteration(stdout, &loop);
            loop.iteration++;

            /*
             * Instead of shaing the entire loop data, the master (which is the
             * only mpi process with the updated maxsqshift) calculates if the
             * loop must be continued and shares only the result with the other
             * processes.
             */
            continue_flag = continue_loop(
                &loop,
                master_args.tolerance,
                master_args.max_iterations,
                master_args.force_iterations
            );
        }
        MPI_Bcast(
            &continue_flag, 1, MPI_INT, MPI_MASTER_RANK, MPI_DEFAULT_COMM
        );
    } while (continue_flag);
    // free the extra buffer
    free_points_collection(&new_centroids);


    if (mpi_is_master(mpi_rank)) {
        // write the output
        finish_loop(stdout, &loop, hpc_gettime());
        write_output_file(&master_args, &master_clusters, &master_points);

        free_clusters_collection(&master_clusters);
        clusters.centroids.data =
            NULL;  // prevent double free of centroids data
        free_points_collection(&master_points);

        master_n_points = safe_free(master_n_points);
        master_points_displacements = safe_free(master_points_displacements);
    }

    // free remaining data
    free_clusters_collection(&clusters);
    free_points_collection(&points);

    // free the mpi types
    MPI_Type_free(&point_type);
    MPI_Type_free(&input_info_type);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
