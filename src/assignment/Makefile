CC?=gcc
MPICC?=mpicc
CFLAGS+=-std=c99 -Wall -Wpedantic
OMP_FLAGS+=-fopenmp

SRC_DIR?=src
BUILD_DIR?=build
BIN_DIR?=bin

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

.PHONY: all serial omp mpi inputgen clean

all: $(BINARIES)

serial: $(SERIAL_BIN)
omp: $(OMP_BIN)
mpi: $(MPI_BIN)
inputgen: $(INPUTGEN_BIN)

clean:
	rm -f $(OBJECTS)
	rm -f $(BINARIES)

# create directories
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# compile objects
$(OMP_OBJ): $(OMP_MAIN) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OMP_FLAGS) -c $< -o $@

$(MPI_OBJ): $(MPI_MAIN) | $(BUILD_DIR)
	$(MPICC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
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
