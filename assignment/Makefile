# utils
comma:=,
space:=$(empty) $(empty)

# --- parameters ---

define list_with_extra
$(1)_EXTRA?=
$(1)?=$(2) $$($(1)_EXTRA)
endef

## host
CORES_SET?=

# this is done here because it's used by other parameters
# set to all cores if CORES_SET is empty
ifeq ($(CORES_SET),)
  NPROC:=$(shell nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 1)
  PROCS=$(strip $(shell seq 0 $$(($(NPROC)-1))))
  CORES_SET:=$(subst $(space),$(comma),$(PROCS))
endif
# get cores count
CORES_COUNT:=$(words $(subst $(comma), ,$(CORES_SET)))
WORK_UNITS?=$(CORES_COUNT)

## programs and options
CC?=gcc
$(eval $(call list_with_extra,CFLAGS,-std=c99 -Wall -Wpedantic))
$(eval $(call list_with_extra,LDFLAGS,,))

$(eval $(call list_with_extra,OMP_CFLAGS,-fopenmp))
$(eval $(call list_with_extra,OMP_LDFLAGS,-fopenmp))
OMP_PLACES?={$(CORES_SET)}
OMP_NUM_THREADS?=$(WORK_UNITS)

MPICC?=mpicc
MPIRUN?=mpirun
$(eval $(call list_with_extra,MPI_CFLAGS,,))
$(eval $(call list_with_extra,MPI_LDFLAGS,,))
$(eval $(call list_with_extra,MPIRUN_FLAGS,-n $(WORK_UNITS) --oversubscribe --cpu-list $(CORES_SET)))

NVCC?=nvcc
$(eval $(call list_with_extra,CUDA_CFLAGS,-Wno-deprecated-gpu-targets))
$(eval $(call list_with_extra,CUDA_LDFLAGS,,))

BEAR?=bear
MAKE?=make
FFMPEG?=ffmpeg

## project
SRC_DIR?=src
BUILD_DIR?=build
BIN_DIR?=bin
INCLUDE_DIR?=include
SCRIPTS_DIR?=scripts

## movie
MAKE_MOVIE?=0
MOVIE_DIR?=movie
GENFRAMES_SCRIPT?=$(SCRIPTS_DIR)/generate_frames.sh

## demo
DEMO_CLUSTERS?=5
DEMO_INPUTGEN_POINTS?=1000
DEMO_INPUTGEN_DIMS?=2
DEMO_INPUTGEN_CLUSTERS?=50
DEMO_INPUT?=demo.in
DEMO_OUTPUT?=demo.out
DEMO_MOVIE?=demo.mp4

## scaling
SCALING_DIR?=scaling
SCALING_SCRIPT?=$(SCRIPTS_DIR)/scaling.sh
SCALING_BASE_POINTS?=10000
SCALING_INPUT_CLUSTERS?=10
SCALING_INPUTGEN_DIMS?=20
SCALING_INPUTGEN_CLUSTERS?=100
SCALING_NREPS?=10
SCALING_MAX_UNITS?=$(WORK_UNITS)

## inputgen
INPUTGEN_MAIN?=$(SCRIPTS_DIR)/inputgen.c
INPUTGEN_BIN?=$(BIN_DIR)/inputgen
INPUTGEN_POINTS?=$(DEMO_INPUTGEN_POINTS)
INPUTGEN_DIMS?=$(DEMO_INPUTGEN_DIMS)
INPUTGEN_CLUSTERS?=$(DEMO_INPUTGEN_CLUSTERS)
INPUTGEN_OUTPUT?=$(DEMO_INPUT)

## arguments
CLUSTERS?=$(DEMO_CLUSTERS)
INPUT?=$(DEMO_INPUT)
OUTPUT?=$(DEMO_OUTPUT)
MOVIE?=$(DEMO_MOVIE)

# --- parameters ---

# sources
define make_src
$(wildcard $(SRC_DIR)/$(1))
endef

SERIAL_MAIN:=$(call make_src,serial-k-means.c)
OMP_MAIN:=$(call make_src,omp-k-means.c)
MPI_MAIN:=$(call make_src,mpi-k-means.c)
CUDA_MAIN:=$(call make_src,cuda-k-means.cu)

MAIN_SRCS:=$(SERIAL_MAIN) $(OMP_MAIN) $(MPI_MAIN) $(CUDA_MAIN)
SERIAL_SRCS:=$(call make_src,serial-*.c)
OMP_SRCS:=$(call make_src,omp-*.c)
MPI_SRCS:=$(call make_src,mpi-*.c)
CUDA_SRCS:=$(wildcard $(call make_src,cuda-*.cu))
COMMON_SRCS:=$(filter-out $(MAIN_SRCS) $(SERIAL_SRCS) $(OMP_SRCS) $(MPI_SRCS) $(CUDA_SRCS),$(call make_src,*.c))

SRCS:=$(sort $(MAIN_SRCS) $(SERIAL_SRCS) $(OMP_SRCS) $(MPI_SRCS) $(CUDA_SRCS) $(COMMON_SRCS))

# object files
define src_to_obj
$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(1:%.cu=%.c))
endef

SERIAL_OBJ:=$(call src_to_obj,$(SERIAL_MAIN))
OMP_OBJ:=$(call src_to_obj,$(OMP_MAIN))
MPI_OBJ:=$(call src_to_obj,$(MPI_MAIN))
CUDA_OBJ:=$(call src_to_obj,$(CUDA_MAIN))

MAIN_OBJS:=$(call src_to_obj,$(MAIN_SRCS))
SERIAL_OBJS:=$(call src_to_obj,$(SERIAL_SRCS))
OMP_OBJS:=$(call src_to_obj,$(OMP_SRCS))
MPI_OBJS:=$(call src_to_obj,$(MPI_SRCS))
CUDA_OBJS:=$(call src_to_obj,$(CUDA_SRCS))
COMMON_OBJS:=$(call src_to_obj,$(COMMON_SRCS))

OBJS:=$(sort $(MAIN_OBJS) $(SERIAL_OBJS) $(OMP_OBJS) $(MPI_OBJS) $(CUDA_OBJS) $(COMMON_OBJS))

# dependencies
define obj_to_deps
$(1:%.o=%.d)
endef

SERIAL_DEP:=$(call obj_to_deps,$(SERIAL_OBJ))
OMP_DEP:=$(call obj_to_deps,$(OMP_OBJ))
MPI_DEP:=$(call obj_to_deps,$(MPI_OBJ))
CUDA_DEP:=$(call obj_to_deps,$(CUDA_OBJ))

MAIN_DEPS:=$(call obj_to_deps,$(MAIN_OBJS))
SERIAL_DEPS:=$(call obj_to_deps,$(SERIAL_OBJS))
OMP_DEPS:=$(call obj_to_deps,$(OMP_OBJS))
MPI_DEPS:=$(call obj_to_deps,$(MPI_OBJS))
CUDA_DEPS:=$(call obj_to_deps,$(CUDA_OBJS))
COMMON_DEPS:=$(call obj_to_deps,$(COMMON_OBJS))

DEPS:=$(sort $(MAIN_DEPS) $(SERIAL_DEPS) $(OMP_DEPS) $(MPI_DEPS) $(CUDA_DEPS) $(COMMON_DEPS))

# binaries
define obj_to_bin
$(1:$(BUILD_DIR)/%.o=$(BIN_DIR)/%)
endef

SERIAL_BIN:=$(call obj_to_bin,$(SERIAL_OBJ))
OMP_BIN:=$(call obj_to_bin,$(OMP_OBJ))
MPI_BIN:=$(call obj_to_bin,$(MPI_OBJ))
CUDA_BIN:=$(call obj_to_bin,$(CUDA_OBJ))

BINS:=$(SERIAL_BIN) $(OMP_BIN) $(MPI_BIN) $(CUDA_BIN)

NODEPS_TARGETS=clean clean-build clean-compiledb clean-movie clean-demo clean-scaling

# targets

# add only targets for which the main source exists
BUILD_TARGETS:=
RUN_TARGETS:=
MOVIE_TARGETS:=
SCALING_TARGETS:=
ifneq ($(wildcard $(SERIAL_MAIN)),)
BUILD_TARGETS+=build-serial
RUN_TARGETS+=run-serial
MOVIE_TARGETS+=movie-serial
SCALING_TARGETS+=scaling-serial-weak scaling-serial-strong
endif
ifneq ($(wildcard $(OMP_MAIN)),)
BUILD_TARGETS+=build-omp
RUN_TARGETS+=run-omp
MOVIE_TARGETS+=movie-omp
SCALING_TARGETS+=scaling-omp-weak scaling-omp-strong
endif
ifneq ($(wildcard $(MPI_MAIN)),)
BUILD_TARGETS+=build-mpi
RUN_TARGETS+=run-mpi
MOVIE_TARGETS+=movie-mpi
SCALING_TARGETS+=scaling-mpi-weak scaling-mpi-strong
endif
ifneq ($(wildcard $(CUDA_MAIN)),)
BUILD_TARGETS+=build-cuda
RUN_TARGETS+=run-cuda
MOVIE_TARGETS+=movie-cuda
# weak and strong scaling are not for cuda
endif

.PHONY: all \
	$(NODEPS_TARGETS) \
	build build-inputgen $(BUILD_TARGETS) \
	run-inputgen $(RUN_TARGETS) \
	$(MOVIE_TARGETS) \
	$(SCALING_TARGETS) \
	demo-input \
	compiledb compiledb-full

all: build

build: build-inputgen $(BUILD_TARGETS)

define make_build_target
ifneq ($(filter build-$(1), $(BUILD_TARGETS)),)
build-$(1): $(2)
endif
endef

$(eval $(call make_build_target,serial,$(SERIAL_BIN)))
$(eval $(call make_build_target,omp,$(OMP_BIN)))
$(eval $(call make_build_target,mpi,$(MPI_BIN)))
$(eval $(call make_build_target,cuda,$(CUDA_BIN)))

build-inputgen: $(INPUTGEN_BIN)

define make_run_target
ifneq ($(filter run-$(1), $(RUN_TARGETS)),)
run-$(1): $(2) $(INPUT)
	MAKE_MOVIE=$(MAKE_MOVIE) MOVIE_DIR=$(MOVIE_DIR) $(3) $(2) $(CLUSTERS) $(INPUT) $(OUTPUT)
endif
endef

OMP_ENV:=OMP_PLACES=$(OMP_PLACES) OMP_NUM_THREADS=$(OMP_NUM_THREADS)
$(eval $(call make_run_target,serial,$(SERIAL_BIN)))
$(eval $(call make_run_target,omp,$(OMP_BIN),$(OMP_ENV)))
$(eval $(call make_run_target,mpi,$(MPI_BIN),$(MPIRUN) $(MPIRUN_FLAGS)))
$(eval $(call make_run_target,cuda,$(CUDA_BIN)))

run-inputgen: $(INPUTGEN_BIN)
	$(INPUTGEN_BIN) $(INPUTGEN_POINTS) $(INPUTGEN_DIMS) $(INPUTGEN_CLUSTERS) > $(INPUTGEN_OUTPUT)

compiledb:
	$(BEAR) --append -- $(MAKE) build

compiledb-full: clean-build
	$(BEAR) -- $(MAKE) build

define make_movie_target
ifneq ($(filter movie-$(1), $(MOVIE_TARGETS)),)
movie-$(1): $(MOVIE_DIR)/ build-$(1)
	$(MAKE) run-$(1) MAKE_MOVIE=1 && \
	$(GENFRAMES_SCRIPT) $(MOVIE_DIR) && \
	$(FFMPEG) -pattern_type glob -stream_loop 5 -y -r 1 -i "$(MOVIE_DIR)/img_*.png" -vcodec mpeg4 -r 1 $(MOVIE)
endif
endef

$(eval $(call make_movie_target,serial))
$(eval $(call make_movie_target,omp))
$(eval $(call make_movie_target,mpi))
$(eval $(call make_movie_target,cuda))

define make_scaling_target
ifneq ($(filter scaling-$(1)-$(2), $(SCALING_TARGETS)),)
scaling-$(1)-$(2): $(SCALING_DIR)/ build-$(1) build-inputgen
	VARIANT=$(1) \
	SCALING_TYPE=$(2) \
	BASE_POINTS=$(SCALING_BASE_POINTS) \
	INPUT_CLUSTERS=$(SCALING_INPUT_CLUSTERS) \
	INPUTGEN_DIMS=$(SCALING_INPUTGEN_DIMS) \
	INPUTGEN_CLUSTERS=$(SCALING_INPUTGEN_CLUSTERS) \
	SCALING_DIR=$(SCALING_DIR) \
	MAKE=$(MAKE) \
	NREPS=$(SCALING_NREPS) \
	MAX_UNITS=$(SCALING_MAX_UNITS) \
	$(SCALING_SCRIPT)
endif
endef

$(eval $(call make_scaling_target,serial,weak))
$(eval $(call make_scaling_target,serial,strong))
$(eval $(call make_scaling_target,omp,weak))
$(eval $(call make_scaling_target,omp,strong))
$(eval $(call make_scaling_target,mpi,weak))
$(eval $(call make_scaling_target,mpi,strong))

clean-build:
	rm -f $(DEPS)
	rm -f $(OBJS)
	rm -f $(BINS)
	rm -f $(INPUTGEN_BIN)

clean-compiledb:
	rm -f compile_commands.json
	rm -f compile_commands.events.json

clean-movie:
	rm -f $(MOVIE_DIR)/clusters_*.txt
	rm -f $(MOVIE_DIR)/img_*.png
	rm -f $(MOVIE_DIR)/centroids_*.txt
	rm -f $(MOVIE)

clean-demo:
	rm -f $(DEMO_INPUT)
	rm -f $(DEMO_OUTPUT)
	rm -f $(DEMO_MOVIE)

clean-scaling:
	rm -f $(SCALING_DIR)/*.in
	rm -f $(SCALING_DIR)/*.out

clean: clean-build clean-compiledb clean-movie clean-demo clean-scaling

demo-input: $(DEMO_INPUT)

# add include directory to compilation flags
CFLAGS+=-I$(INCLUDE_DIR)

# create directories
$(BUILD_DIR)/ $(BIN_DIR)/ $(MOVIE_DIR)/ $(SCALING_DIR)/:
	mkdir -p $@

# create demo files
$(DEMO_INPUT):
	$(MAKE) run-inputgen \
	    INPUTGEN_POINTS=$(DEMO_INPUTGEN_POINTS) \
	    INPUTGEN_DIMS=$(DEMO_INPUTGEN_DIMS) \
		INPUTGEN_CLUSTERS=$(DEMO_INPUTGEN_CLUSTERS) \
		INPUTGEN_OUTPUT=$(DEMO_INPUT)

# override compilers and flags
SERIAL_TARGETS:=$(SERIAL_BIN) $(SERIAL_SRCS) $(SERIAL_OBJS) $(SERIAL_DEPS)
OMP_TARGETS:=$(OMP_BIN) $(OMP_SRCS) $(OMP_OBJS) $(OMP_DEPS)
MPI_TARGETS:=$(MPI_BIN) $(MPI_SRCS) $(MPI_OBJS) $(MPI_DEPS)
CUDA_TARGETS:=$(CUDA_BIN) $(CUDA_SRCS) $(CUDA_OBJS) $(CUDA_DEPS)

$(SERIAL_TARGETS): EXTRA_OBJS+=$(SERIAL_OBJS)

$(OMP_TARGETS): private CFLAGS+=$(OMP_CFLAGS)
$(OMP_TARGETS): private LDFLAGS+=$(OMP_LDFLAGS)
$(OMP_TARGETS): private EXTRA_OBJS+=$(OMP_OBJS)

$(MPI_TARGETS): private CC:=$(MPICC)
$(MPI_TARGETS): private CFLAGS+=$(MPI_CFLAGS)
$(MPI_TARGETS): private LDFLAGS+=$(MPI_LDFLAGS)
$(MPI_TARGETS): private EXTRA_OBJS+=$(MPI_OBJS)

$(CUDA_TARGETS): private CC:=$(NVCC)
$(CUDA_TARGETS): private CFLAGS+=$(CUDA_CFLAGS)
$(CUDA_TARGETS): private LDFLAGS+=$(CUDA_LDFLAGS)
# C std not supported by nvcc
$(CUDA_TARGETS): private CFLAGS:=$(filter-out -std=%,$(CFLAGS))
# -Wpedantic creates warning with autogenerated code in preprocessing of nvcc
$(CUDA_TARGETS): private CFLAGS:=$(filter-out -Wpedantic,$(CFLAGS))
$(CUDA_TARGETS): private CFLAGS:=$(foreach flag,$(CFLAGS),-Xcompiler $(flag))

# compile inputgen
$(INPUTGEN_BIN): $(INPUTGEN_MAIN) | $(BIN_DIR)/
	$(CC) $(CFLAGS) $^ -o $@

# compile dependencies
# https://gcc.gnu.org/onlinedocs/gcc-4.3.2/cpp/Invocation.html
.SECONDEXPANSION:
$(BUILD_DIR)/%.d: $$(wildcard $(SRC_DIR)/%.c $(SRC_DIR)/%.cu) | $(BUILD_DIR)/
	$(CC) $(CFLAGS) -MM -MT $(call src_to_obj,$<) $< -MF $@

ifeq ($(filter $(MAKECMDGOALS),$(NODEPS_TARGETS)),)
-include $(DEPS)
endif

# compile objects
$(BUILD_DIR)/%.o: $$(wildcard $(SRC_DIR)/%.c $(SRC_DIR)/%.cu) $(BUILD_DIR)/%.d | $(BUILD_DIR)/
	$(CC) $(CFLAGS) -c $< -o $@

# compile binaries
$(BIN_DIR)/%: $(BUILD_DIR)/%.o $$(COMMON_OBJS) $$(EXTRA_OBJS) | $(BIN_DIR)/
	$(CC) $(LDFLAGS) $^ -o $@
