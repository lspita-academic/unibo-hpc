// Ludovico Maria Spitaleri 0001114169

/*
 * defining _XOPEN_SOURCE first allows hpc.h to not be the first header
 * included, so autoformatters can be used
 */
#if _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600
#endif

#include <stdio.h>
#include <stdlib.h>

// #include "hpc.h"
#include "safety.h"

int main(void) {
  printf("Hello, World!\n");
  safe_exit(EXIT_SUCCESS);
}
