# HPC 2025/26 Final project

This repository contains the source code for the final project of the HPC 2025/26 course.

> [!IMPORTANT]
> Even if the makefile specification can handle CUDA, only the OpenMP and MPI variants are implemented, as requested by the assignment.

The serial version provided by the professor was not used, instead it was reimplemented by maintaining the same core structure while making the code more reusable also by the other implementations.

> [!IMPORTANT]
> The term "variant" refers to the different implementations of the algorithms: serial, omp, mpi or cuda.
> The make targets for each specific variant exist only if the corresponding main source file is present, that is
>
> - `serial`: `src/serial-k-means.c`
> - `omp`: `src/omp-k-means.c`
> - `mpi`: `src/mpi-k-means.c`
> - `cuda`: `src/cuda-k-means.cu`

## Build

### Make

```sh
make build # build everything. This is the default target.
# or
make build-<variant> # build a specific variant of the program.
```

> [!IMPORTANT]
> Each variant compiles with its specific source files:
>
> - `serial-*.c`: Sources included only in the serial variant.
> - `omp-*.c`: Sources that use OpenMP, included only in the omp variant.
> - `mpi-*.c`: Sources that use MPI, included only in the mpi variant.
> - `cuda-*.cu`: Sources that use CUDA, included only in the cuda variant.
>
> All other sources are considered standard C with no special requirements and are shared between all variants.

## Run

### Make

```sh
make run-<variant>
```

Parameters:

- `CLUSTERS`: Number of clusters (default: $DEMO_CLUSTERS)
- `INPUT`: Input file path (default: $DEMO_INPUT)
- `OUTPUT`: Output file path (default: $DEMO_OUTPUT)

> [!TIP]
> The `run` targets automatically build all the necessary files they need, so it's not required to run the corresponding `build` target before.
> Moreover, since the demo input can be auto generated, they are an effective way to test the algorithm with zero configuration.

### Manual

All binaries are stored in the [BIN_DIR](#parameters) directory and follow the assignment specification for the parameters.

```sh
./bin/<variant>-k-means <CLUSTERS> <INPUT> <OUTPUT>
```

## Movie

To create a movie of the algorithm, a make target is provided:

```sh
make movie-<variant>
```

Parameters:

- `MOVIE_DIR`: Directory for temporary movie files (default: movie)
- `CLUSTERS`: Number of clusters (default: $DEMO_CLUSTERS)
- `INPUT`: Input file path (default: $DEMO_INPUT)
- `OUTPUT`: Output file path (default: $DEMO_OUTPUT)

## Demo

To manually generate the demo input file, a target is provided:

```sh
make demo-input
```

Parameters:

- `DEMO_INPUTGEN_POINTS`: Number of points to use (default: 20)
- `DEMO_INPUTGEN_DIMS`: Number of dimensions for each point (default: 2)
- `DEMO_INPUTGEN_CLUSTERS`: Number of clusters to use (default: 50)
- `DEMO_INPUT`: File to generate (default: demo.in)

## Scaling

A script to measure weak and strong scaling performances is provided.

> [!IMPORTANT]
> The serial variant can be measured using the scaling script, fixed to one core only. It's useful just to measure the time in the same conditions of the parallel versions to get data for comparisons.
> On the other hand, the scaling script doesn't support the cuda variant, since it's not a useful benchmark.

```sh
make scaling-<variant>-<type>
```

Parameters:

- `<type>`: Scaling type, either weak or strong.
- `SCALING_BASE_POINTS`: Starting input points (default: 10000).
- `SCALING_INPUT_CLUSTERS`: Number of clusters (default: 10).
- `SCALING_INPUTGEN_DIMS`: Number of dimensions for each point (default: 20).
- `SCALING_INPUTGEN_CLUSTERS`: Number of clusters to use for input generation (default: 100).
- `SCALING_NREPS`: Number of iterations to do on each round (default: 5).
- `SCALING_MAX_UNITS`: Max number of work units to use (default: $WORK_UNITS).

## Inputgen

An independent binary for input generation (provided by the professor) is also present among the other targets.

This is included to allow the autogeneration of the demo input file to always have an example to run even if no file is provided.

### Build

```sh
make build-inputgen
```

### Run

#### Make

```sh
make run-inputgen
```

##### Parameters

- `INPUTGEN_POINTS`: Number of points to generate (default: $DEMO_INPUTGEN_POINTS).
- `INPUTGEN_DIMS`: Number of dimensions for each point (default: $DEMO_INPUTGEN_DIMS).
- `INPUTGEN_CLUSTERS`: Number of clusters to use (default: $DEMO_INPUTGEN_CLUSTERS).
- `INPUTGEN_OUTPUT`: Output file to use (default: $DEMO_INPUT)

##### Manual

The binary is stored in the [BIN_DIR](#parameters) alongside the others.

```sh
./bin/inputgen <POINTS> <DIMS> <CLUSTERS> # the output is printed to stdout
```

Parameters:

- `POINTS`: Number of points to generate (required)
- `DIMS`: Number of dimensions of each point (required)
- `CLUSTERS`: Number of clusters to use (required)

## Clean

Artifacts cleaning is splitted into multiple targets.

```sh
make clean-build # cleans all build files and compiled binaries
make clean-compiledb # cleans the compilation database
make clean-movie # cleans all movie files
make clean-demo # cleans all demo files
make clean-scaling # cleans all scaling files
make clean # cleans all artifacts previously mentioned
```

## Notes

### Formatting style

The code formatting is based on the Google C++ style guide provided by the [clang-format preset](https://clang.llvm.org/docs/ClangFormatStyleOptions.html#basedonstyle) with slight modifications.

### Compilation database

Make targets to create a compilation database from the build target for clangd are provided.

```sh
make compiledb # compiles what has changed and updates the compiledb
# or
make compiledb-full # cleans build artifacts and recompiles all sources for a clean compiledb
```

This target requires [bear](https://github.com/rizsotto/Bear) to be installed.

Here's the diff: the **Scaling** subsection in Parameters only has `SCALING_BASE_POINTS`, but the Makefile has 5 more scaling vars. Also `MPI_CFLAGS`, `MPI_LDFLAGS`, `CUDA_CFLAGS`, `CUDA_LDFLAGS` all use `list_with_extra` so they should be documented with their `_EXTRA` variants. Here's the full section to paste:

---

### Parameters

The parameters used by the targets are simple makefile variables. They can be overriden with environment variables using the same name or by providing them after the target name.

```sh
CLUSTERS=... make run-omp # using environment variables
# or
make run-omp CLUSTERS=... # providing them to the target manually
```

> [!NOTE]
> When a parameter has an "EXTRA" equivalent, it means it is a list where you can add extra values on top of the default ones by setting the corresponding extra variable.

Here a complete list of parameters. If anything is missing, the [Makefile](./Makefile) can always be consulted.

### Host

- `CORES_SET`: Comma-separated list of cores to use (default: all cores).
- `WORK_UNITS`: Number of work units to use (default: number of elements in `CORES_SET`)

### Programs and options

- `CC`: Compiler for standard c sources (default: gcc).
- `CFLAGS/CFLAGS_EXTRA`: Flags to use when compiling all sources (default: -std=c99 -Wall -Wpedantic).
- `LDFLAGS/LDFLAGS_EXTRA`: Flags to use when linking all sources (default: empty).
- `OMP_CFLAGS/OMP_CFLAGS_EXTRA`: Flags to add when compiling OpenMP sources (default: -fopenmp).
- `OMP_LDFLAGS/OMP_LDFLAGS_EXTRA`: Flags to add when linking OpenMP sources (default: -fopenmp).
- `OMP_PLACES`: OpenMP OMP_PLACES env var (default: {$CORES_SET})
- `OMP_NUM_THREADS`: OpenMP OMP_NUM_THREADS env var (default: $WORK_UNITS)
- `MPICC`: Compiler for MPI sources (default: mpicc).
- `MPIRUN`: Wrapper to use to run MPI binaries (default: mpirun).
- `MPI_CFLAGS/MPI_CFLAGS_EXTRA`: Flags to add when compiling MPI sources (default: none).
- `MPI_LDFLAGS/MPI_LDFLAGS_EXTRA`: Flags to add when linking MPI sources (default: none).
- `MPIRUN_FLAGS/MPIRUN_FLAGS_EXTRA`: Flags to add to the wrapper when running MPI binaries (default: -n $WORK_UNITS --oversubscribe --cpu-list $CORES_SET).
- `NVCC`: Compiler for CUDA sources (default: nvcc).
- `CUDA_CFLAGS/CUDA_CFLAGS_EXTRA`: Flags to add when compiling CUDA sources (default: -Wno-deprecated-gpu-targets).
- `CUDA_LDFLAGS/CUDA_LDFLAGS_EXTRA`: Flags to add when linking CUDA sources (default: none).
- `BEAR`: Compiler for compiledb (default: bear).
- `MAKE`: Make program to use when running targets inside targets, e.g. inside the demo target (default: make).

### Project

- `SRC_DIR`: Directory containing the source files (default: src).
- `BUILD_DIR`: Directory to use for build artifacts (default: build).
- `BIN_DIR`: Directory to use for compiled binaries (default: bin).
- `INCLUDE_DIR`: Directory containing external headers (default: include).
- `SCRIPTS_DIR`: Directory containing extra scripts (default: scripts).

### Movie

- `MAKE_MOVIE`: Set to 1 to enable frame generation during a run (default: 0). Set automatically by `movie-<variant>` targets.
- `MOVIE_DIR`: Directory to store temporary movie files (default: movie).
- `GENFRAMES_SCRIPT`: Script used for movie frames generation (default: $SCRIPTS_DIR//generate_frames.sh)

### Demo

- `DEMO_CLUSTERS`: Number of clusters for the demo (default: 5).
- `DEMO_INPUTGEN_POINTS`: Number of points for the demo (default: 1000).
- `DEMO_INPUTGEN_DIMS`: Number of dimensions for the demo (default: 2).
- `DEMO_INPUTGEN_CLUSTERS`: Number of clusters for the demo (default: 50).
- `DEMO_INPUT`: Input file for the demo (default: demo.in, autogenerated if missing).
- `DEMO_OUTPUT`: Output file for the demo (default: demo.out).
- `DEMO_MOVIE`: Movie file for the demo (default: demo.mp4).

### Scaling

- `SCALING_DIR`: Directory to store temporary scaling files (default: scaling).
- `SCALING_SCRIPT`: Script to use for scaling benchmarking (default: $SCRIPTS_DIR/scaling.sh)
- `SCALING_BASE_POINTS`: Starting input points (default: 10000).
- `SCALING_INPUT_CLUSTERS`: Number of clusters (default: 10).
- `SCALING_INPUTGEN_DIMS`: Number of dimensions for each point (default: 20).
- `SCALING_INPUTGEN_CLUSTERS`: Number of clusters to use for input generation (default: 100).
- `SCALING_NREPS`: Number of iterations to do on each round (default: 5).
- `SCALING_MAX_UNITS`: Max number of work units to use (default: $WORK_UNITS).

### Inputgen

- `INPUTGEN_MAIN`: Main source file of inputgen (default: $SCRIPTS_DIR/inputgen.c).
- `INPUTGEN_BIN`: Path to the inputgen binary (default: $BIN_DIR/inputgen).
- `INPUTGEN_POINTS`: Number of points to generate (default: $DEMO_INPUTGEN_POINTS).
- `INPUTGEN_DIMS`: Number of dimensions for each point (default: $DEMO_INPUTGEN_DIMS).
- `INPUTGEN_CLUSTERS`: Number of clusters to use (default: $DEMO_INPUTGEN_CLUSTERS).
- `INPUTGEN_OUTPUT`: Output file to use (default: $DEMO_INPUT)

### Arguments

- `CLUSTERS`: Number of clusters (default: $DEMO_CLUSTERS)
- `INPUT`: Input file to use (default: $DEMO_INPUT)
- `OUTPUT`: Output file to use (default: $DEMO_OUTPUT)
- `MOVIE`: Movie file to use (default: $DEMO_MOVIE)
