#!/usr/bin/env bash

required_vars=( \
    VARIANT \
    SCALING_TYPE \
    BASE_POINTS \
    INPUT_CLUSTERS \
    INPUTGEN_DIMS \
    INPUTGEN_CLUSTERS \
    SCALING_DIR \
    MAKE \
    NREPS \
    MAX_UNITS \
)

for var in "${required_vars[@]}"; do
  if [[ -z "${!var}" ]]; then
    echo "Error: $var is not set or empty." 1>&2
    exit 1
  fi
done

if [ $BASE_POINTS -le 0 ]; then
    echo "Invalid base problem size: $BASE_POINTS" 1>&2
    exit 1
fi

if [[ "$VARIANT" == "serial" ]]; then
    MAX_UNITS=1
fi

mkdir -p $SCALING_DIR

table_header="p\tn"
for i in $(seq 1 $NREPS); do table_header="$table_header\tt$i"; done
echo -e "$table_header"

for p in $(seq 1 $MAX_UNITS); do
    ENV_VARS=()
    case ${SCALING_TYPE} in
        weak)
            INPUT_POINTS=$(( BASE_POINTS * p ))
            ENV_VARS+=(FORCE_ITERATIONS=true)
            ;;
        strong)
            INPUT_POINTS=$BASE_POINTS
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
        OUTPUT_FILE=${FILE_BASENAME}-${SCALING_TYPE}-${VARIANT}-K${INPUT_CLUSTERS}-P${p}-R${rep}.out
        EXEC_TIME="$(
            env ${ENV_VARS[@]} $MAKE run-$VARIANT WORK_UNITS=$p CLUSTERS=$INPUT_CLUSTERS INPUT=$INPUT_FILE OUTPUT=$OUTPUT_FILE | \
            grep "Elapsed seconds: " | \
            sed 's/Elapsed seconds: //' \
        )"
        echo -n -e "${EXEC_TIME}\t"
    done
    echo ""
done
