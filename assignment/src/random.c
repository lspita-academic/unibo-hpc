// Ludovico Maria Spitaleri 0001114169

#include "random.h"

#include <stdbool.h>
#include <stdlib.h>

#include "abort.h"

bool __RANDOM_INITIALIZED = false;

void init_random(void) {
    srand(RAND_SEED);
    __RANDOM_INITIALIZED = true;
}

int rand_int(void) {
    safe_assert(
        __RANDOM_INITIALIZED,
        "Random number generator used before initialization"
    );
    return rand();
}

int rand_int_range(int min, int max) {
    return (rand_int() % (max - min + 1)) + min;
}
