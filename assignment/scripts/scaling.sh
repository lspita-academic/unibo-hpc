#!/usr/bin/env bash

WORKDIR=$(echo "$(git rev-parse --show-toplevel 2>/dev/null)/assignment" || pwd)

INPUT_BASE_POINTS=${1:-${INPUT_BASE_POINTS:-1}} # base problem size
SCALING_TYPE=${2:-${SCALING_TYPE:-"strong"}} # scaling type, either "weak" or "strong"
BIN_TYPE=${3:-${BIN_TYPE:-"serial"}} # binary type, either "serial", "omp" or "mpi". Serial is just to get the table of values for comparison.
INPUT_CLUSTERS=${4:-${INPUT_CLUSTERS:-5}} # number of clusters
NREPS=${5:-${NREPS:-5}} # number of replications

if [ $INPUT_BASE_POINTS -le 0 ]; then
    echo "Invalid base problem size: $INPUT_BASE_POINTS" 1>&2
    exit 1
fi

if [[ "$BIN_TYPE" == "serial" ]]; then
    MAX_CORES=1
else
    MAX_CORES=${MAX_CORES:-$(cat /proc/cpuinfo | grep processor | wc -l)} # number of (logical) cores
fi

BIN_DIR=${BIN_DIR:-$WORKDIR/bin}
SCALING_DIR=${SCALING_DIR:-$WORKDIR/scaling}
INPUTGEN_BIN=${INPUTGEN_BIN:-$BIN_DIR/inputgen}
INPUTGEN_DIMS=${INPUTGEN_DIMS:-20}
INPUTGEN_CLUSTERS=${INPUTGEN_CLUSTERS:-50}

BINARY=${BIN_DIR}/${BIN_TYPE}-k-means

if [ ! -f "$BINARY" ] || [ ! -f "$INPUTGEN_BIN" ]; then
    echo "$BINARY not found" 1>&2
    exit 1
fi

mkdir -p $SCALING_DIR

table_header="p\tn"
for i in $(seq 1 $NREPS); do table_header="$table_header\tt$i"; done
echo -e "$table_header"

for p in $(seq 1 $MAX_CORES); do
    case ${SCALING_TYPE} in
        weak)
            INPUT_POINTS=$(( INPUT_BASE_POINTS * p ))
            ;;
        strong)
            INPUT_POINTS=$INPUT_BASE_POINTS
            ;;
    esac

    case ${BIN_TYPE} in
        omp)
            PREFIX=(env OMP_NUM_THREADS=$p)
            ;;
        mpi)
            PREFIX=(mpirun -n $p)
            ;;
        serial)
            PREFIX=()
            ;;
    esac

    FILE_BASENAME=${SCALING_DIR}/test-N${INPUT_POINTS}-D${INPUTGEN_DIMS}-C${INPUTGEN_CLUSTERS}
    INPUT_FILE=$FILE_BASENAME.in
    if [ ! -f "$INPUT_FILE" ]; then
        $INPUTGEN_BIN $INPUT_POINTS $INPUTGEN_DIMS $INPUTGEN_CLUSTERS > $INPUT_FILE
    fi

    echo -n -e "$p\t"
    echo -n -e "$INPUT_POINTS\t"
    for rep in `seq $NREPS`; do
        OUTPUT_FILE=${FILE_BASENAME}-${SCALING_TYPE}-${BIN_TYPE}-P${p}-R${rep}.out
        EXEC_TIME="$( ${PREFIX[@]} $BINARY $INPUT_CLUSTERS $INPUT_FILE $OUTPUT_FILE | grep "Elapsed seconds:" | sed 's/Elapsed seconds: //' )"
        echo -n -e "${EXEC_TIME}\t"
    done
    echo ""
done
