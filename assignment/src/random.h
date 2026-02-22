// Ludovico Maria Spitaleri 0001114169

#ifndef RANDOM_H
#define RANDOM_H

#define RAND_SEED 123

/**
 * Initialize the random number generator.
 */
void init_random(void);

/**
 * Generate a random integer.
 */
int rand_int(void);

/**
 * Generate a random integer in the inclusive range [min, max].
 */
int rand_int_range(int min, int max);

#endif  // RANDOM_H
