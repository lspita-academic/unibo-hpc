# programs and flags
CC?=gcc
MPICC?=mpicc
NVCC?=nvcc

CFLAGS+=-std=c99 -Wall -Wpedantic
OMP_FLAGS+=-fopenmp
NVCC_FLAGS+=-Wno-deprecated-gpu-targets

MPIRUN?=mpirun
MPIRUN_FLAGS+=

MAKE?=make

# directories
SRC_DIR?=src
BUILD_DIR?=build
BIN_DIR?=bin
SCRIPTS_DIR?=scripts

# demo
DEMO_DIR?=demo
DEMO_K?=5
DEMO_POINTS?=20
DEMO_CLUSTERS?=50
GENERATE_FRAMES_SCRIPT?=generate_frames.sh
DEMO_INPUT?=demo.in
DEMO_OUTPUT?=demo.out

# Arguments
K?=$(DEMO_K)
INPUT?=$(DEMO_INPUT)
OUTPUT?=$(DEMO_OUTPUT)

# sources
define make_src
$(SRC_DIR)/$(1)
endef

SERIAL_MAIN:=$(call make_src,serial-k-means.c)
OMP_MAIN:=$(call make_src,omp-k-means.c)
MPI_MAIN:=$(call make_src,mpi-k-means.c)
CUDA_MAIN:=$(call make_src,cuda-k-means.cu)
INPUTGEN_MAIN:=$(call make_src,inputgen.c)

MAIN_SRCS:=$(SERIAL_MAIN) $(OMP_MAIN) $(MPI_MAIN) $(CUDA_MAIN) $(INPUTGEN_MAIN)
COMMON_SRCS:=$(filter-out $(MAIN_SRCS),$(wildcard $(SRC_DIR)/*.c $(SRC_DIR)/*.cu))

SRCS:=$(MAIN_SRCS) $(COMMON_SRCS)

# object files
define src_to_obj
$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(1:%.cu=%.c))
endef

SERIAL_OBJ:=$(call src_to_obj,$(SERIAL_MAIN))
OMP_OBJ:=$(call src_to_obj,$(OMP_MAIN))
MPI_OBJ:=$(call src_to_obj,$(MPI_MAIN))
CUDA_OBJ:=$(call src_to_obj,$(CUDA_MAIN))
INPUTGEN_OBJ:=$(call src_to_obj,$(INPUTGEN_MAIN))

MAIN_OBJS:=$(call src_to_obj,$(MAIN_SRCS))
COMMON_OBJS:=$(call src_to_obj,$(COMMON_SRCS))

OBJS:=$(MAIN_OBJS) $(COMMON_OBJS)

# dependencies
define obj_to_deps
$(1:%.o=%.d)
endef

SERIAL_DEPS:=$(call obj_to_deps,$(SERIAL_OBJ))
OMP_DEPS:=$(call obj_to_deps,$(OMP_OBJ))
MPI_DEPS:=$(call obj_to_deps,$(MPI_OBJ))
CUDA_DEPS:=$(call obj_to_deps,$(CUDA_OBJ))
INPUTGEN_DEPS:=$(call obj_to_deps,$(INPUTGEN_OBJ))

MAIN_DEPS:=$(call obj_to_deps,$(MAIN_OBJS))
COMMON_DEPS:=$(call obj_to_deps,$(COMMON_OBJS))

DEPS:=$(MAIN_DEPS) $(COMMON_DEPS)

# binaries
define obj_to_bin
$(1:$(BUILD_DIR)/%.o=$(BIN_DIR)/%)
endef

SERIAL_BIN=$(call obj_to_bin,$(SERIAL_OBJ))
OMP_BIN=$(call obj_to_bin,$(OMP_OBJ))
MPI_BIN=$(call obj_to_bin,$(MPI_OBJ))
CUDA_BIN=$(call obj_to_bin,$(CUDA_OBJ))
INPUTGEN_BIN=$(call obj_to_bin,$(INPUTGEN_OBJ))

BINS:=$(SERIAL_BIN) $(OMP_BIN) $(MPI_BIN) $(CUDA_BIN) $(INPUTGEN_BIN)

# targets
.PHONY: all build build-serial build-omp build-mpi build-cuda build-inputgen run-inputgen demo-input

all: build

build: build-serial build-omp build-mpi build-inputgen

build-serial: $(SERIAL_BIN)
build-omp: $(OMP_BIN)
build-mpi: $(MPI_BIN)
build-cuda: $(CUDA_BIN)
build-inputgen: $(INPUTGEN_BIN)

define make_run_target
.PHONY: run-$(1)
run-$(1): $(2)
	$(3) $(2) $(K) $(INPUT) $(OUTPUT)
endef

$(eval $(call make_run_target,serial,$(SERIAL_BIN)))
$(eval $(call make_run_target,omp,$(OMP_BIN)))
$(eval $(call make_run_target,mpi,$(MPI_BIN),$(MPIRUN) $(MPIRUN_FLAGS)))
$(eval $(call make_run_target,cuda,$(CUDA_BIN)))

run-inputgen: $(INPUTGEN_BIN)
	$(INPUTGEN_BIN) $(POINTS) $(DIMS) $(CLUSTERS) > $(OUTFILE)

define make_demo_target
.PHONY: demo-$(1)
demo-$(1): $(DEMO_INPUT) run-$(1)
	MAKE_MOVIE=1 DEMO_DIR=$(DEMO_DIR) $(MAKE) run-$(1) K=$(DEMO_K) INPUT=$(DEMO_INPUT) OUTPUT=$(DEMO_OUTPUT)
	$(SCRIPTS_DIR)/$(GENERATE_FRAMES_SCRIPT) $(DEMO_DIR)
	ffmpeg -pattern_type glob -stream_loop 5 -y -r 1 -i "$(DEMO_DIR)/img_*.png" -vcodec mpeg4 -r 1 $(DEMO_DIR)/demo_$(1).mp4
endef

$(eval $(call make_demo_target,serial))
$(eval $(call make_demo_target,omp))
$(eval $(call make_demo_target,mpi))
$(eval $(call make_demo_target,cuda))

demo-input: $(DEMO_INPUT)

clean:
	rm -f $(OBJS)
	rm -f $(BINS)
	rm -f $(DEPS)
	rm -f $(DEMO_DIR)/{clusters_*.txt,img_*.png,centroids_*.txt,demo_*.mp4}

# create directories
$(BUILD_DIR)/ $(BIN_DIR)/ $(DEMO_DIR)/ $(TESTS_DIR)/:
	mkdir -p $@

# create demo files
$(DEMO_INPUT): run-inputgen | $(DEMO_DIR)/
	$(MAKE) run-inputgen POINTS=$(DEMO_POINTS) DIMS=2 CLUSTERS=$(DEMO_CLUSTERS) OUTFILE=$@

# override compilers and flags
define targets_matching
$(filter $(BUILD_DIR)/$(1).o,$(OBJS)) \
$(filter $(BUILD_DIR)/$(1).d,$(DEPS)) \
$(filter $(BIN_DIR)/$(1),$(BINS)) \
$(filter $(SRC_DIR)/$(1).c,$(SRCS)) \
$(filter $(SRC_DIR)/$(1).cu,$(SRCS))
endef

OMP_TARGETS:=$(call targets_matching,omp-%)
MPI_TARGETS:=$(call targets_matching,mpi-%)
CUDA_TARGETS:=$(call targets_matching,cuda-%)

$(OMP_TARGETS): CFLAGS+=$(OMP_FLAGS)

$(MPI_TARGETS): CC:=$(MPICC)

$(CUDA_TARGETS): CC:=$(NVCC)
$(CUDA_TARGETS): CFLAGS:=$(filter-out -std=%,$(CFLAGS))
$(CUDA_TARGETS): CFLAGS+=$(CUDA_FLAGS)

$(INPUTGEN_BIN): COMMON_OBJS:=

# compile dependencies
# https://gcc.gnu.org/onlinedocs/gcc-4.3.2/cpp/Invocation.html
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.c | $(BUILD_DIR)/
	$(CC) $(CFLAGS) -MM -MQ '$(call src_to_obj,$<)' -c $< -MF $@

-include $(DEPS)

# compile objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)/%.d | $(BUILD_DIR)/
	$(CC) $(CFLAGS) -c $< -o $@

# compile binaries
$(BIN_DIR)/%: $(BUILD_DIR)/%.o $(COMMON_OBJS) | $(BIN_DIR)/
	$(CC) $(CFLAGS) $^ -o $@
