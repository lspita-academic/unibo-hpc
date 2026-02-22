// Ludovico Maria Spitaleri 0001114169

#include "random.h"

#include <stdlib.h>

void init_random(void) { srand(RAND_SEED); }

int rand_int(void) { return rand(); }

int rand_int_range(int min, int max) {
    return (rand_int() % (max - min + 1)) + min;
}
