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
