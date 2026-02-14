# compilation and flags
CC?=gcc
MPICC?=mpicc
CFLAGS+=-std=c99 -Wall -Wpedantic
OMP_FLAGS+=-fopenmp
MPIRUN=mpirun
MPI_PROCESSES?=8
MPIRUN_FLAGS=-n $(MPI_PROCESSES)
MAKE?=make

# directories
SRC_DIR?=src
BUILD_DIR?=build
BIN_DIR?=bin
TESTS_DIR?=tests
SCRIPTS_DIR?=scripts

# demo
DEMO_DIR?=demo
DEMO_K?=5
DEMO_POINTS?=20
DEMO_CLUSTERS?=50
GENERATE_FRAMES_SCRIPT?=$(SCRIPTS_DIR)/generate_frames.sh
DEMO_TEST?=demo

# test files
define make_test_in
$(TESTS_DIR)/$(1).in
endef

define make_test_out
$(TESTS_DIR)/$(1).out
endef

DEMO_IN:=$(call make_test_in,$(DEMO_TEST))
DEMO_OUT:=$(call make_test_out,$(DEMO_TEST))

TEST?=$(DEMO_TEST)
TEST_K?=$(DEMO_K)
TEST_IN:=$(call make_test_in,$(TEST))
TEST_OUT:=$(call make_test_out,$(TEST))

# main sources
define make_src
$(SRC_DIR)/$(1).c
endef

SERIAL_MAIN=$(call make_src,k-means-serial)
OMP_MAIN=$(call make_src,k-means-omp)
MPI_MAIN=$(call make_src,k-means-mpi)
INPUTGEN_MAIN=$(call make_src,inputgen)

# sources
MAIN_SOURCES:=$(SERIAL_MAIN) $(OMP_MAIN) $(MPI_MAIN) $(INPUTGEN_MAIN)
COMMON_SOURCES:=$(filter-out $(MAIN_SOURCES),$(wildcard $(SRC_DIR)/*.c))

SOURCES:=$(MAIN_SOURCES) $(COMMON_SOURCES)

# object files
define src_to_obj
$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(1))
endef

COMMON_OBJS:=$(call src_to_obj,$(COMMON_SOURCES))
SERIAL_OBJ:=$(call src_to_obj,$(SERIAL_MAIN))
OMP_OBJ:=$(call src_to_obj,$(OMP_MAIN))
MPI_OBJ:=$(call src_to_obj,$(MPI_MAIN))
INPUTGEN_OBJ:=$(call src_to_obj,$(INPUTGEN_MAIN))

OBJECTS:=$(COMMON_OBJS) $(SERIAL_OBJ) $(OMP_OBJ) $(MPI_OBJ) $(INPUTGEN_OBJ)

# binaries
define src_to_bin
$(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(1))
endef

SERIAL_BIN=$(call src_to_bin,$(SERIAL_MAIN))
OMP_BIN=$(call src_to_bin,$(OMP_MAIN))
MPI_BIN=$(call src_to_bin,$(MPI_MAIN))
INPUTGEN_BIN=$(call src_to_bin,$(INPUTGEN_MAIN))

BINARIES:=$(SERIAL_BIN) $(OMP_BIN) $(MPI_BIN) $(INPUTGEN_BIN)

# targets
.PHONY: all build-all build-serial build-omp build-mpi build-inputgen

all: build-all

build-all: build-serial build-omp build-mpi build-inputgen

build-serial: $(SERIAL_BIN)
build-omp: $(OMP_BIN)
build-mpi: $(MPI_BIN)
build-inputgen: $(INPUTGEN_BIN)

define make_run_target
.PHONY: run-$(1)
run-$(1): $(2)
	$(3) $(2) $(TEST_K) $(TEST_IN) $(TEST_OUT)
endef

$(eval $(call make_run_target,serial,$(SERIAL_BIN)))
$(eval $(call make_run_target,omp,$(OMP_BIN)))
$(eval $(call make_run_target,mpi,$(MPI_BIN),$(MPIRUN) $(MPIRUN_FLAGS)))

define make_demo_target
.PHONY: demo-$(1)
demo-$(1): $(DEMO_INPUT) run-$(1)
	MAKE_MOVIE=1 DEMO_DIR=$(DEMO_DIR) TEST=$(DEMO_TEST) $(MAKE) run-$(1)
	$(GENERATE_FRAMES_SCRIPT) $(DEMO_DIR)
	ffmpeg -pattern_type glob -stream_loop 5 -y -r 1 -i "$(DEMO_DIR)/img_*.png" -vcodec mpeg4 -r 1 $(TESTS_DIR)/demo-$(1).mp4
endef

$(eval $(call make_demo_target,serial))
$(eval $(call make_demo_target,omp))
$(eval $(call make_demo_target,mpi))

clean:
	rm -f $(OBJECTS)
	rm -f $(BINARIES)
	rm -f $(DEMO_DIR)/*

# create directories
$(BUILD_DIR) $(BIN_DIR) $(DEMO_DIR) $(TESTS_DIR):
	mkdir -p $@

# compile objects
$(OMP_OBJ): $(OMP_MAIN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OMP_FLAGS) -c $< -o $@

$(MPI_OBJ): $(MPI_MAIN) | $(BUILD_DIR)
	$(MPICC) $(CFLAGS) -c $< -o $@

$(SERIAL_OBJ) $(INPUTGEN_OBJ) $(COMMON_OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile binaries
$(SERIAL_BIN): $(SERIAL_OBJ) $(COMMON_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OMP_BIN): $(OMP_OBJ) $(COMMON_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OMP_FLAGS) $^ -o $@

$(MPI_BIN): $(MPI_OBJ) $(COMMON_OBJS) | $(BIN_DIR)
	$(MPICC) $(CFLAGS) $^ -o $@

$(INPUTGEN_BIN): $(INPUTGEN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# create demo files
$(DEMO_IN): $(INPUTGEN_BIN) | $(DEMO_DIR)
	$< $(DEMO_POINTS) 2 $(DEMO_CLUSTERS) > $@
